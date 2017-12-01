#include "audiocapture.h"

AudioCapture::AudioCapture(AudioBuffer* audioBuffer)
{
    _audioBuffer = audioBuffer;

    _formatBit = 16;
    _pcmName = "hw:1,0";
    _rate = 44100;

    _samplesize = (_formatBit == 24) ? (32 / 8) : (_formatBit / 8);

    _channels = 2;
    _framesize = _channels * _samplesize;
    _periods = 4;
    _periodsize = 4096;
}

AudioCapture::~AudioCapture()
{
    _doCapture = false;
    _captureThread.join();
}

void AudioCapture::run()
{
    int pcmreturn;

    _doCapture = true;

    if (!(_captureHandle = open_pcm())) {
        // ("Could not open PCM for capture.");
        _doCapture = false;
        return;
    }

    _periodBuf = new char[_periodsize * _framesize];
    _audioBuffer->resetBuffers();

    struct sched_param schp;
    memset(&schp, 0, sizeof(schp));
    schp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (int err = sched_setscheduler(0, SCHED_FIFO, &schp) != 0) {
        //("Can't set sched_setscheduler (%d) - using normal priority", err);
    }

    while (_doCapture) {
        while ((pcmreturn = snd_pcm_readi(_captureHandle, _periodBuf,
                        _periodsize)) < 0) {
            snd_pcm_prepare(_captureHandle);
        }

        int count = _periodsize * _framesize; // number of values to write to ring buffer
        _audioBuffer->writeAudioToBuffers(_periodBuf, count, _channels, _formatBit);
    }
    snd_pcm_close(_captureHandle);
    free(_periodBuf);
}

void AudioCapture::start()
{
    _doCapture = true;
    _captureThread = thread(&AudioCapture::run, this);
}

void AudioCapture::stop()
{
    _doCapture = false;
}

snd_pcm_t *AudioCapture::open_pcm()
{
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_uframes_t buffersize_return;
    unsigned int tmp;
    int err;

    err = snd_pcm_open(&pcm_handle, _pcmName.c_str(),
            SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
//        ("Error opening PCM device %s (%s)", (_pcmName),
//                snd_strerror(err));
        return(NULL);
    }

    snd_pcm_hw_params_alloca(&hwparams);
    err = snd_pcm_hw_params_any(pcm_handle, hwparams);
    if (err < 0) {
//        ("Can not configure this PCM device (%s).", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return(NULL);
    }
    err = snd_pcm_hw_params_set_access(pcm_handle, hwparams,
                SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
//        ("Error setting access (%s).", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return(NULL);
    }
    if (_formatBit == 16) {
        err = snd_pcm_hw_params_set_format(pcm_handle, hwparams,
                    SND_PCM_FORMAT_S16_LE);
        if (err < 0) {
            snd_pcm_close(pcm_handle);
//            ("Error setting format (%s).", snd_strerror(err));
            return(NULL);
        }
    } else if (_formatBit == 24) {
        err = snd_pcm_hw_params_set_format(pcm_handle, hwparams,
                    SND_PCM_FORMAT_S24_LE);
        if (err < 0) {
            snd_pcm_close(pcm_handle);
//            ("Error setting format (%s).", snd_strerror(err));
            return(NULL);
        }
    } else {
        err = snd_pcm_hw_params_set_format(pcm_handle, hwparams,
                    SND_PCM_FORMAT_S32_LE);
        if (err < 0) {
            snd_pcm_close(pcm_handle);
//            ("Error setting format (%s).", snd_strerror(err));
            return(NULL);
        }
    }
    tmp = (unsigned int) _rate;
    err = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &tmp, 0);
    if (err < 0) {
//        ("Error setting rate (%s).", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return(NULL);
    }

    if ((unsigned int) _rate != tmp) {
//         Rate " << _rate << " changed to " << tmp;
    }

    err = snd_pcm_hw_params_set_channels(pcm_handle, hwparams, _channels);
    if (err < 0) {
        snd_pcm_close(pcm_handle);
//        ("Error setting channels (%s).", snd_strerror(err));
        return(NULL);
    }
    err = snd_pcm_hw_params_set_periods(pcm_handle, hwparams, _periods, 0);
    if (err < 0) {
        snd_pcm_close(pcm_handle);
//        ("Error setting periods (%s).", snd_strerror(err));
        return(NULL);
    }
    buffersize_return = _periodsize * _periods;
    err = snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams,
                    &buffersize_return);
    if (err < 0) {
//        ("Error setting buffersize (%s).", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return(NULL);
    }
    if (buffersize_return != (snd_pcm_uframes_t)(_periodsize * _periods)) {
//        ("Periodsize %d is not available on your hardware. "
//            "Using %d instead.",
//            _periodsize, (int)buffersize_return / _periods);
        _periodsize = buffersize_return / _periods;
    }
    err = snd_pcm_hw_params(pcm_handle, hwparams);
    if (err < 0) {
//        ("Error setting HW params (%s).", snd_strerror(err));
        snd_pcm_close(pcm_handle);
        return(NULL);
    }

    return(pcm_handle);
}
