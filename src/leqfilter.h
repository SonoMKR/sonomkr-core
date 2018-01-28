#ifndef LEQ_FILTER_H
#define LEQ_FILTER_H

#include <math.h>
#include <atomic>
#include <iostream>
#include <libconfig.h++>

#include "ringbuffer.h"
#include "ringbufferconsumer.h"
#include "iirfilter.h"

using namespace std;
using namespace libconfig;

class LeqFilter: public RingBufferConsumer<float> {
private:
    int _sampleRate;
    float _integrationPeriod; // in seconds
    int _resetPeriod; // in samples
    unsigned int _readCounter;
    float _accumulator;

    IirFilter _filter;
    float* _leqBuffer;
    int _leqBufferSize;

    //only one reader
    int _leqReaderPosition;
    int _leqWritePosition;
    atomic<int> _leqSizeReadable;

    int processData(unsigned long readPosition);

public:
    LeqFilter(RingBuffer<float>* buffer,
              int sizeToRead,
              int rate,
              float integrationPeriode,
              Setting& filterConfig
             );
    ~LeqFilter();

    bool beginReadLeq(const int& sizeToRead, int& readPosition);
    void endReadLeq(const int& sizeRed);

    float* getLeqBuffer() {
        return _leqBuffer;
    }
};

#endif // LEQ_FILTER_H
