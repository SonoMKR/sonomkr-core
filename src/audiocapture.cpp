#include "audiocapture.h"

#include <iostream>

AudioCapture::AudioCapture(Configuration *config, AudioBuffer *audio_buffer) :
    config_(config),
    audio_buffer_(audio_buffer)
{
    bit_depth_ = config_->audio_.bit_depth;
    pcm_name_ = config_->audio_.sound_card;
    sample_rate_ = config_->audio_.sample_rate;
    channels_ = config_->audio_.available_channels;
    periods_ = config_->audio_.periods;
    period_size_ = config_->audio_.period_size;

    sample_size_ = (bit_depth_ == 24) ? (32 / 8) : (bit_depth_ / 8);
    frame_size_ = channels_ * sample_size_;
}

AudioCapture::~AudioCapture()
{
    do_capture_ = false;
    if (capture_thread_.joinable()) {
        capture_thread_.join();
    }
}

void AudioCapture::run()
{
    int pcmreturn;

    do_capture_ = true;

    if (!(capture_handle_ = open_pcm()))
    {
        std::cerr << ("Could not open PCM for capture.") << std::endl;
        do_capture_ = false;
        return;
    }

    period_buf_ = new char[period_size_ * frame_size_];
    audio_buffer_->resetBuffers();

    struct sched_param schp;
    memset(&schp, 0, sizeof(schp));
    schp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (int err = sched_setscheduler(0, SCHED_FIFO, &schp) != 0)
    {
        std::cerr << "Can't set sched_setscheduler (" << err << ") - using normal priority" << std::endl;
    }

    while (do_capture_)
    {
        while ((pcmreturn = snd_pcm_readi(capture_handle_, period_buf_,
                                          period_size_)) < 0)
        {
            snd_pcm_prepare(capture_handle_);
            // Buffer Overrun
        }

        int count = period_size_ * frame_size_; // number of values to write to ring buffer

        audio_buffer_->writeAudioToBuffers(period_buf_, count, channels_, bit_depth_);
    }
    snd_pcm_close(capture_handle_);
    free(period_buf_);
}

void AudioCapture::start()
{
    do_capture_ = true;
    capture_thread_ = std::thread(&AudioCapture::run, this);
}

void AudioCapture::stop()
{
    do_capture_ = false;
    if (capture_thread_.joinable()) {
        capture_thread_.join();
    }
}

snd_pcm_t *AudioCapture::open_pcm()
{
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_uframes_t buffersize_return;
    unsigned int tmp;
    int err;

    err = snd_pcm_open(&pcm_handle, pcm_name_.c_str(),
                       SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0)
    {
        std::cerr << "Error opening PCM device" << pcm_name_ << "(" << snd_strerror(err) << ")" << std::endl;
        return (NULL);
    }

    snd_pcm_hw_params_alloca(&hwparams);
    err = snd_pcm_hw_params_any(pcm_handle, hwparams);
    if (err < 0)
    {
        //        ("Can not configure this PCM device (%s).", snd_strerror(err));
        std::cerr << "Can not configure this PCM device (" << snd_strerror(err) << ")" << std::endl;
        snd_pcm_close(pcm_handle);
        return (NULL);
    }
    err = snd_pcm_hw_params_set_access(pcm_handle, hwparams,
                                       SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
    {
        //        ("Error setting access (%s).", snd_strerror(err));
        std::cerr << "Error setting access (" << snd_strerror(err) << ")" << std::endl;
        snd_pcm_close(pcm_handle);
        return (NULL);
    }
    if (bit_depth_ == 16)
    {
        err = snd_pcm_hw_params_set_format(pcm_handle, hwparams,
                                           SND_PCM_FORMAT_S16_LE);
        if (err < 0)
        {
            snd_pcm_close(pcm_handle);
            std::cerr << "Error setting format (" << snd_strerror(err) << ")" << std::endl;
            //            ("Error setting format (%s).", snd_strerror(err));
            return (NULL);
        }
    }
    else if (bit_depth_ == 24)
    {
        err = snd_pcm_hw_params_set_format(pcm_handle, hwparams,
                                           SND_PCM_FORMAT_S24_LE);
        if (err < 0)
        {
            snd_pcm_close(pcm_handle);
            //            ("Error setting format (%s).", snd_strerror(err));
            std::cerr << "Error setting format (" << snd_strerror(err) << ")" << std::endl;
            return (NULL);
        }
    }
    else
    {
        err = snd_pcm_hw_params_set_format(pcm_handle, hwparams,
                                           SND_PCM_FORMAT_S32_LE);
        if (err < 0)
        {
            snd_pcm_close(pcm_handle);
            //            ("Error setting format (%s).", snd_strerror(err));
            std::cerr << "Error setting format (" << snd_strerror(err) << ")" << std::endl;
            return (NULL);
        }
    }
    tmp = (unsigned int)sample_rate_;
    err = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &tmp, 0);
    if (err < 0)
    {
        //        ("Error setting rate (%s).", snd_strerror(err));
        std::cerr << "Error setting rate (" << snd_strerror(err) << ")" << std::endl;
        snd_pcm_close(pcm_handle);
        return (NULL);
    }

    if ((unsigned int)sample_rate_ != tmp)
    {
        //         Rate " << _rate << " changed to " << tmp;
    }

    err = snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channels_);
    if (err < 0)
    {
        snd_pcm_close(pcm_handle);
        //        ("Error setting channels (%s).", snd_strerror(err));
        std::cerr << "Error setting channels (" << snd_strerror(err) << ")" << std::endl;
        return (NULL);
    }
    err = snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods_, 0);
    if (err < 0)
    {
        snd_pcm_close(pcm_handle);
        //        ("Error setting periods (%s).", snd_strerror(err));
        std::cerr << "Error setting periods (" << snd_strerror(err) << ")" << std::endl;
        return (NULL);
    }
    buffersize_return = period_size_ * periods_;
    err = snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams,
                                                 &buffersize_return);
    if (err < 0)
    {
        //        ("Error setting buffersize (%s).", snd_strerror(err));
        std::cerr << "Error setting buffersize (" << snd_strerror(err) << ")" << std::endl;
        snd_pcm_close(pcm_handle);
        return (NULL);
    }
    if (buffersize_return != (snd_pcm_uframes_t)(period_size_ * periods_))
    {
        //        ("Periodsize %d is not available on your hardware. "
        //            "Using %d instead.",
        //            period_size_, (int)buffersize_return / _periods);
        std::cerr << "Periodsize " << period_size_ << " is not available on your hardware. Using " << ((int)buffersize_return / periods_) << "instead" << std::endl;
        period_size_ = buffersize_return / periods_;
    }
    err = snd_pcm_hw_params(pcm_handle, hwparams);
    if (err < 0)
    {
        //        ("Error setting HW params (%s).", snd_strerror(err));
        std::cerr << "Error setting HW params (" << snd_strerror(err) << ")" << std::endl;
        snd_pcm_close(pcm_handle);
        return (NULL);
    }

    return (pcm_handle);
}
