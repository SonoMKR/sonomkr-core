#ifndef ANTIALIASINGFILTER_H
#define ANTIALIASINGFILTER_H

#include "../Shared/ringbuffer.h"
#include "../Shared/ringbufferconsumer.h"
#include "../SignalProcessing/sos_coefficients.h"
#include "iirfilter.h"

using namespace std;

class AntiAliasingFilter: public RingBufferConsumer<float>
{
private:
    int _inputSampleRate;
    int _ouputSampleRate;

    IirFilter _filter;
    int _sampleCounter;

    RingBuffer<float>* _outputBuffer;
    ulong _outputBufferSize;

    int processData(ulong readPosition);

public:
    AntiAliasingFilter(RingBuffer<float> *inputBuffer,
            int sizeToRead,
            int inputSampleRate);
    ~AntiAliasingFilter();

    RingBuffer<float>* getOutputBuffer() {
        return _outputBuffer;
    }
    ulong getOutputBufferSize() {
        return _outputBufferSize;
    }
};

#endif // ANTIALIASINGFILTER_H
