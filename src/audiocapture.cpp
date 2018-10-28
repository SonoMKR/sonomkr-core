#include "audiocapture.h"

AudioCapture::AudioCapture(Configuration *config, AudioBuffer *audio_buffer) : config_(config),
                                                                               audio_buffer_(audio_buffer)
{
    bit_depth_ = config_->getSetting(string(AUDIO_BITDEPTH_PATH));
    pcm_name_ = config_->getSetting(string(AUDIO_SOUNDCARD_PATH)).c_str();
    sample_rate_ = config_->getSetting(string(AUDIO_SAMPLERATE_PATH));
    channels_ = config_->getSetting(string(AUDIO_CHANNELS_PATH));
    periods_ = config_->getSetting(string(AUDIO_PERIODS_PATH));
    period_size_ = config_->getSetting(string(AUDIO_PERIODSIZE_PATH));

    sample_size_ = (bit_depth_ == 24) ? (32 / 8) : (bit_depth_ / 8);
    frame_size_ = channels_ * sample_size_;
}

AudioCapture::~AudioCapture()
{
    do_capture_ = false;
    capture_thread_.join();
}

void AudioCapture::run()
{
    int pcmreturn;

    do_capture_ = true;

    if (!(capture_handle_ = open_pcm()))
    {
        // ("Could not open PCM for capture.");
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
        //("Can't set sched_setscheduler (%d) - using normal priority", err);
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
    capture_thread_ = thread(&AudioCapture::run, this);
}

void AudioCapture::stop()
{
    do_capture_ = false;
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
        //        ("Error opening PCM device %s (%s)", (pcm_name_),
        //                snd_strerror(err));
        return (NULL);
    }

    snd_pcm_hw_params_alloca(&hwparams);
    err = snd_pcm_hw_params_any(pcm_handle, hwparams);
    if (err < 0)
    {
        //        ("Can not configure this PCM device (%s).", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return (NULL);
    }
    err = snd_pcm_hw_params_set_access(pcm_handle, hwparams,
                                       SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
    {
        //        ("Error setting access (%s).", snd_strerror(err));
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
            return (NULL);
        }
    }
    tmp = (unsigned int)sample_rate_;
    err = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &tmp, 0);
    if (err < 0)
    {
        //        ("Error setting rate (%s).", snd_strerror(err));
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
        return (NULL);
    }
    err = snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods_, 0);
    if (err < 0)
    {
        snd_pcm_close(pcm_handle);
        //        ("Error setting periods (%s).", snd_strerror(err));
        return (NULL);
    }
    buffersize_return = period_size_ * periods_;
    err = snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams,
                                                 &buffersize_return);
    if (err < 0)
    {
        //        ("Error setting buffersize (%s).", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return (NULL);
    }
    if (buffersize_return != (snd_pcm_uframes_t)(period_size_ * periods_))
    {
        //        ("Periodsize %d is not available on your hardware. "
        //            "Using %d instead.",
        //            period_size_, (int)buffersize_return / _periods);
        period_size_ = buffersize_return / periods_;
    }
    err = snd_pcm_hw_params(pcm_handle, hwparams);
    if (err < 0)
    {
        //        ("Error setting HW params (%s).", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return (NULL);
    }

    return (pcm_handle);
}
