#ifndef AUDIOCAPTURE_H
#define AUDIOCAPTURE_H

#include <iostream>

#include <thread>
#include <string>
#include <alsa/asoundlib.h>

#include "configuration.h"
#include "audiobuffer.h"

using namespace std;

class AudioCapture {
private:
    Configuration* _config;
    AudioBuffer* _audioBuffer;
    thread _captureThread;
    snd_pcm_t* _captureHandle;
    char* _periodBuf;
    string _pcmName;
    int _samplesize, _sampleRate, _framesize, _channels, _periodsize, _periods, _bitDepth;
    bool _doCapture;

    snd_pcm_t* open_pcm();

public:
    AudioCapture(Configuration* config, AudioBuffer* audioBuffer);
    ~AudioCapture();
    void run();
    void start();
    void stop();
};

#endif // AUDIOCAPTURE_H
