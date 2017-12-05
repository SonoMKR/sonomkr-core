#include "antialiasingfilter.h"

AntiAliasingFilter::AntiAliasingFilter(
        RingBuffer<float> *inputBuffer,
        int sizeToRead,
        int inputSampleRate):
    RingBufferConsumer<float>(inputBuffer, sizeToRead),
    _inputSampleRate(inputSampleRate),
    _filter(NB_SOS, sos::SOS_ALIASING),
    _sampleCounter(0)
{
    _ouputSampleRate = _inputSampleRate / 10;
    _outputBufferSize = _bufferSize / 10;
    _outputBuffer = new RingBuffer<float>(_outputBufferSize);
}

AntiAliasingFilter::~AntiAliasingFilter()
{
    delete _outputBuffer;
}

int AntiAliasingFilter::processData(ulong readPosition)
{
    int sizeToWrite = 0;
    // carefull to have a _sizeToTead a multiple of 10
    float decimateBuffer[_sizeToRead/10];
    for (int i = 0; i < _sizeToRead; i++)
    {
        ulong position = (readPosition + i) % _bufferSize;
        float sample = _filter.filter(_bufferPtr[position]);
        if (_sampleCounter % 10 == 0)
        {
            decimateBuffer[sizeToWrite] = sample;
            sizeToWrite++;
        }
        _sampleCounter = (_sampleCounter + 1) % 10;
    }

    _outputBuffer->writeToBuffer(decimateBuffer, sizeToWrite);

    return _sizeToRead;
}
