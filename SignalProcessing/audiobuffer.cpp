#include "audiobuffer.h"

AudioBuffer::AudioBuffer(int nbChannels, ulong size):
    _bufferSize(size),
    _nbChannels(nbChannels)
{
    _channelBuffers.push_back(new RingBuffer<float>(_bufferSize));
    if (nbChannels >= 2) {
        _channelBuffers.push_back(new RingBuffer<float>(_bufferSize));
    }
}

AudioBuffer::~AudioBuffer()
{

}

void AudioBuffer::resetBuffers()
{
    for (int c = 0; c < _nbChannels; ++c) {
        _channelBuffers[c]->resetBuffer();
    }
}


RingBuffer<float> *AudioBuffer::getChannelBuffer(int channel)
{
    if (channel > _channelBuffers.size()) {
        channel =  _channelBuffers.size();
    }
    return _channelBuffers[channel];
}


void AudioBuffer::writeAudioToBuffers(const char *inputBuffer, const int &bufferSize, int& nbChannels, int& formatBit)
{
    if (formatBit != 24 && formatBit != 16) {
        return; // format not supported
    }
    if (_nbChannels > nbChannels) {
        return; // should not happen
    }

    int sampleByteSize = (formatBit == 24) ? (32 / 8) : (formatBit / 8);
    int frameByteSize = nbChannels * sampleByteSize;
    int nbSamples = bufferSize / frameByteSize;

    float audioBuffers[_nbChannels][nbSamples];

    int s = 0;

    for (int i = 0; i < bufferSize; i += frameByteSize) {
        for (int c = 0; c < _nbChannels; ++c) {
            float sample = 0.0;
            if (formatBit == 24) {
                sample = decodeAudio24bit(&inputBuffer[i + c * sampleByteSize]);
            }
            if (formatBit == 16) {
                sample = decodeAudio16bit(&inputBuffer[i + c * sampleByteSize]);
            }
            audioBuffers[c][s] = sample;
        }
        ++s;
    }

    for (int c = 0; c < _nbChannels; ++c) {
        _channelBuffers[c]->writeToBuffer(audioBuffers[c], nbSamples);
    }
}

float AudioBuffer::decodeAudio24bit(const char *inputBuffer)
{
    int32_t dataSample32bit = 0;
    float dataSampleSound = 0.0;

    dataSample32bit = ((inputBuffer[0]) | (inputBuffer[1] << 8) | (inputBuffer[2] << 16));
    if ((dataSample32bit & (1U << 23)) > 0) //24 bit negative value
    {
        dataSample32bit |= (0xff << 24);//get 32 bit negative value
    }
    dataSampleSound = (dataSample32bit / 8388608.0); // / 0.05;

    return dataSampleSound;
}

float AudioBuffer::decodeAudio16bit(const char *inputBuffer)
{
    int16_t dataSample16bit = 0;
    float dataSampleSound = 0.0;

    dataSample16bit = ((inputBuffer[0]) | (inputBuffer[1] << 8));
    dataSampleSound = (dataSample16bit / 32768.0); // / 0.05;

    return dataSampleSound;
}
