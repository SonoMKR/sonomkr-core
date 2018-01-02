#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

//#define SINE_TEST
#define SINE_FREQ 160.0
#define SINE_RATE 44100.0

#include <vector>
#include <math.h>

#include "../Shared/ringbuffer.h"
#include "../configuration.h"

class AudioBuffer {
private:
    Configuration* _config;
    ulong _bufferSize;
    int _nbChannels;
    vector<RingBuffer<float>*> _channelBuffers;

#ifdef SINE_TEST
    double _lastTime;
#endif

    float decodeAudio24bit(const char* inputBuffer);
    float decodeAudio16bit(const char* inputBuffer);

public:
    AudioBuffer(Configuration* config);
    ~AudioBuffer();

    void resetBuffers();
    void writeAudioToBuffers(const char* inputBuffer, const int& sizeToWrite, int& nbChannels,
                             int& formatBit);
    RingBuffer<float>* getChannelBuffer(int channel);
};

#endif // AUDIOBUFFER_H
