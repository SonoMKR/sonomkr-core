#include "leqfilter.h"

LeqFilter::LeqFilter(RingBuffer<float>* buffer,
                     int sizeToRead, int rate,
                     float integrationPeriode,
                     Setting& filterConfig
                    ):
    RingBufferConsumer<float>(buffer, sizeToRead),
    _filter(filterConfig),
    _sampleRate(rate),
    _integrationPeriod(integrationPeriode),
    _accumulator(0.0),
    _readCounter(0)
{
    _resetPeriod = int(_sampleRate * _integrationPeriod);
    _leqBufferSize = int(300 / _integrationPeriod); // allways 300 seconds => 5 min
    _leqBuffer = new float[_leqBufferSize];

    _leqReaderPosition = 0;
    _leqWritePosition = 0;
    _leqSizeReadable.store(0);
}

LeqFilter::~LeqFilter()
{
    delete[] _leqBuffer;
}

int LeqFilter::processData(ulong readPosition)
{
    double sample;
    for (int i = 0; i < _sizeToRead; ++i) {
        sample = _filter.filter(_bufferPtr[(i + readPosition) % _bufferSize]);
        _accumulator += sample * sample;
        _readCounter++;

        if (_readCounter >= _resetPeriod) {
//            std::cout << 10.0 * log10((sqrt(_accumulator / _readCounter))) << std::endl;
            _leqBuffer[_leqWritePosition] = 10.0 * log10((sqrt(_accumulator / _readCounter)));
            _leqWritePosition = (_leqWritePosition + 1) % _leqBufferSize;
            _leqSizeReadable.store(_leqSizeReadable.load() + 1);
            _readCounter %= _resetPeriod;
            _accumulator = 0.0;
        }
    }

    return _sizeToRead;
}

bool LeqFilter::beginReadLeq(const int& sizeToRead, int& readPosition)
{
    if (_leqSizeReadable.load() < sizeToRead) {
        return false;
    }
    readPosition = _leqReaderPosition;
    return true;
}

void LeqFilter::endReadLeq(const int& sizeRed)
{
    _leqReaderPosition = (_leqReaderPosition + sizeRed) % _leqBufferSize;
    _leqSizeReadable.store(_leqSizeReadable.load() - sizeRed);
}
