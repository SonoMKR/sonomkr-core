#ifndef ANTIALIASINGFILTER_H
#define ANTIALIASINGFILTER_H

#include <libconfig.h++>

#include "ringbuffer.h"
#include "ringbufferconsumer.h"
#include "iirfilter.h"

class AntiAliasingFilter : public RingBufferConsumer
{
  private:
    int input_sample_sate_;
    int ouput_sample_rate_;
    IirFilter filter_;
    int sample_counter_;
    RingBuffer *output_buffer_;
    unsigned long output_buffer_size_;

    int processData(unsigned long read_position);

  public:
    AntiAliasingFilter(RingBuffer *input_buffer,
                       int size_to_read,
                       int input_sample_rate,
                       libconfig::Setting &filter_config);
    ~AntiAliasingFilter();

    RingBuffer *getOutputBuffer()
    {
        return output_buffer_;
    }
    unsigned long getOutputBufferSize()
    {
        return output_buffer_size_;
    }
};

#endif // ANTIALIASINGFILTER_H
