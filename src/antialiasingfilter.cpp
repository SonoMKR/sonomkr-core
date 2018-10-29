#include "antialiasingfilter.h"

AntiAliasingFilter::AntiAliasingFilter(RingBuffer *input_buffer,
                                       int size_to_read,
                                       int input_sample_rate,
                                       libconfig::Setting &filter_config) :
    RingBufferConsumer(input_buffer, size_to_read),
    input_sample_sate_(input_sample_rate),
    filter_(filter_config),
    sample_counter_(0),
    ouput_sample_rate_(input_sample_rate / 10),
    output_buffer_size_(buffer_size_ / 10)
{
    output_buffer_ = new RingBuffer(output_buffer_size_);
}

AntiAliasingFilter::~AntiAliasingFilter()
{
    delete output_buffer_;
}

int AntiAliasingFilter::processData(unsigned long read_position)
{
    int size_to_write = 0;
    // carefull to have a _sizeToTead a multiple of 10
    float decimate_buffer[size_to_read_ / 10];
    for (int i = 0; i < size_to_read_; i++)
    {
        unsigned long position = (read_position + i) % buffer_size_;
        float sample = filter_.filter(buffer_ptr_[position]);
        if (sample_counter_ % 10 == 0)
        {
            decimate_buffer[size_to_write] = sample;
            size_to_write++;
        }
        sample_counter_ = (sample_counter_ + 1) % 10;
    }

    output_buffer_->writeToBuffer(decimate_buffer, size_to_write);

    return size_to_read_;
}
