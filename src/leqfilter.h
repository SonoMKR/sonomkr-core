#ifndef LEQ_FILTER_H
#define LEQ_FILTER_H

#include <math.h>
#include <atomic>
#include <iostream>
#include <libconfig.h++>

#include "ringbuffer.h"
#include "ringbufferconsumer.h"
#include "iirfilter.h"

class LeqFilter : public RingBufferConsumer
{
  private:
    int sample_rate_;
    float integration_period_; // in seconds
    int reset_period_;         // in samples
    unsigned int read_counter_;
    float accumulator_;

    IirFilter filter_;
    float *leq_buffer_;
    int leq_buffer_size_;

    //only one reader
    int leq_reader_position_;
    int leq_write_position_;
    std::atomic<int> leq_size_readable_;

    int processData(unsigned long read_position);

  public:
    LeqFilter(RingBuffer *buffer,
              int size_to_read,
              int rate,
              float integration_periode,
              libconfig::Setting &filter_config);
    ~LeqFilter();

    bool beginReadLeq(const int &size_to_read, int &read_position);
    void endReadLeq(const int &size_red);

    inline float *getLeqBuffer()
    {
        return leq_buffer_;
    }
};

#endif // LEQ_FILTER_H
