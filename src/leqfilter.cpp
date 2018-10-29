#include "leqfilter.h"

LeqFilter::LeqFilter(RingBuffer *buffer,
                     int size_to_read, int rate,
                     float integration_periode,
                     libconfig::Setting &filter_config) :
    RingBufferConsumer(buffer, size_to_read),
    filter_(filter_config),
    sample_rate_(rate),
    integration_period_(integration_periode),
    accumulator_(0.0),
    read_counter_(0)
{
    reset_period_ = int(sample_rate_ * integration_period_);
    leq_buffer_size_ = int(300 / integration_period_); // allways 300 seconds => 5 min
    leq_buffer_ = new float[leq_buffer_size_];

    leq_reader_position_ = 0;
    leq_write_position_ = 0;
    leq_size_readable_.store(0);
}

LeqFilter::~LeqFilter()
{
    delete[] leq_buffer_;
}

int LeqFilter::processData(unsigned long read_position)
{
    double sample;
    for (int i = 0; i < size_to_read_; ++i)
    {
        sample = filter_.filter(buffer_ptr_[(i + read_position) % buffer_size_]);
        accumulator_ += sample * sample;
        read_counter_++;

        if (read_counter_ >= reset_period_)
        {
            leq_buffer_[leq_write_position_] = 10.0 * log10((sqrt(accumulator_ / read_counter_)));
            leq_write_position_ = (leq_write_position_ + 1) % leq_buffer_size_;
            leq_size_readable_.store(leq_size_readable_.load() + 1);
            read_counter_ %= reset_period_;
            accumulator_ = 0.0;
        }
    }
    return size_to_read_;
}

bool LeqFilter::beginReadLeq(const int &size_to_read, int &read_position)
{
    if (leq_size_readable_.load() < size_to_read)
    {
        return false;
    }
    read_position = leq_reader_position_;
    return true;
}

void LeqFilter::endReadLeq(const int &size_red)
{
    leq_reader_position_ = (leq_reader_position_ + size_red) % leq_buffer_size_;
    leq_size_readable_.store(leq_size_readable_.load() - size_red);
}
