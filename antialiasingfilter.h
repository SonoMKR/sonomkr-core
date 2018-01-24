#ifndef ANTIALIASINGFILTER_H
#define ANTIALIASINGFILTER_H

#include <libconfig.h++>

#include "ringbuffer.h"
#include "ringbufferconsumer.h"
#include "iirfilter.h"

using namespace std;
using namespace libconfig;

class AntiAliasingFilter: public RingBufferConsumer<float> {
private:
    int _inputSampleRate;
    int _ouputSampleRate;

    IirFilter _filter;
    int _sampleCounter;

    RingBuffer<float>* _outputBuffer;
    ulong _outputBufferSize;

    int processData(ulong readPosition);

public:
    AntiAliasingFilter(RingBuffer<float>* inputBuffer,
                       int sizeToRead,
                       int inputSampleRate,
                       Setting& filterConfig);
    ~AntiAliasingFilter();

    RingBuffer<float>* getOutputBuffer() {
        return _outputBuffer;
    }
    ulong getOutputBufferSize() {
        return _outputBufferSize;
    }
};

#endif // ANTIALIASINGFILTER_H
