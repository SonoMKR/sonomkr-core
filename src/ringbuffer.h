#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <iostream>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <chrono>

struct BufferReader
{
    unsigned long read_position;
    std::atomic<unsigned int> size_readable;

    BufferReader(unsigned long start_position) :
        read_position(start_position),
        size_readable(0)
    {
    }
};

class RingBuffer
{
  private:
    std::shared_timed_mutex mutex_;
    std::condition_variable_any notify_;
    std::chrono::milliseconds ms_timeout_;
    double *buffer_;
    unsigned long buffer_size_;
    unsigned long write_position_;

    std::vector<BufferReader *> readers_list_;

  public:
    RingBuffer(const unsigned long &size);
    ~RingBuffer();
    unsigned long registerReader();
    bool waitToBeginRead(const unsigned long &reader_index, const unsigned long &size_to_read,
                         unsigned long &read_position);
    void endRead(const unsigned long &reader_index, const unsigned int &size_red);
    void writeToBuffer(const double *input_buffer, const unsigned int &size_to_write);
    void resetBuffer();

    inline double *getBufferPtr()
    {
        return buffer_;
    }
    inline unsigned long getBufferSize()
    {
        return buffer_size_;
    }
};

#endif // RINGBUFFER_H
