#include "leq.h"

Leq::Leq(RingBuffer<float>* buffer, int sizeToRead, int nbFilters, double sosCoefficients[][6]):
    RingBufferConsumer<float>(buffer, sizeToRead),
    _filter(nbFilters, sosCoefficients),
    _leqBuffer(300)
{

}

Leq::~Leq()
{

}

int Leq::processData(ulong readPosition)
{
    return _sizeToRead;
}
