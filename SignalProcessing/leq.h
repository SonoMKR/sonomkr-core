#ifndef LEQ_H
#define LEQ_H

#include "../Shared/ringbuffer.h"
#include "../Shared/ringbufferconsumer.h"
#include "iirfilter.h"

class Leq: public RingBufferConsumer<float>
{
private:
    IirFilter _filter;
    RingBuffer<float> _leqBuffer;

    int processData(ulong readPosition);

public:
    Leq(RingBuffer<float> *buffer, int sizeToRead, int nbFilters, double sosCoefficients[][6]);
    ~Leq();
};

#endif // LEQ_H
