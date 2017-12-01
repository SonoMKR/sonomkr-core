#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <vector>

#include "../Shared/ringbuffer.h"

class AudioBuffer
{
private:
    ulong _bufferSize;
    int _nbChannels;
    vector<RingBuffer<float>*> _channelBuffers;

    float decodeAudio24bit(const char *inputBuffer);
    float decodeAudio16bit(const char *inputBuffer);

public:
    AudioBuffer(int nbChannels, ulong size);
    ~AudioBuffer();

    void resetBuffers();
    void writeAudioToBuffers(const char *inputBuffer, const int &sizeToWrite, int &nbChannels, int &formatBit);
    RingBuffer<float>* getChannelBuffer(int channel);
};

#endif // AUDIOBUFFER_H
