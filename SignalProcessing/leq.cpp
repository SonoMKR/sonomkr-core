#include "leq.h"

Leq::Leq(RingBuffer<float>* buffer, int sizeToRead, int nbFilters, double sosCoefficients[][6]):
    RingBufferConsumer<float>(buffer, sizeToRead),
    _filter(nbFilters, sosCoefficients),
    _leqBuffer(300),
    _accumulator(0.0),
    _readCounter(0)
{

}

Leq::~Leq()
{

}

int Leq::processData(ulong readPosition)
{
    double sample;
    for (int i = 0; i < _sizeToRead; ++i) {
        sample = _filter.filter(_bufferPtr[(i + readPosition) % _bufferSize]);
        _accumulator += sample * sample;
        _readCounter++;
    }

    if (_readCounter >= 48000) {
        _readCounter %= 48000;
        std::cout << 20.0 * log10(sqrt(_accumulator / _readCounter) / 0.00002) << std::endl;
        _accumulator = 0.0;
    }

    return _sizeToRead;
}
