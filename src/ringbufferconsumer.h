#ifndef RINGBUFFERCONSUMER_H
#define RINGBUFFERCONSUMER_H

#include <thread>

#include "ringbuffer.h"

class RingBufferConsumer
{
  protected:
    RingBuffer *buffer_;
    int reader_index_;
    std::thread read_thread_;
    bool do_read_;
    float *buffer_ptr_;
    int buffer_size_;
    int size_to_read_;

    virtual int processData(unsigned long read_position) = 0;

  public:
    RingBufferConsumer(RingBuffer *buffer, int size_to_read);
    virtual ~RingBufferConsumer();
    void run();
    void start();
    void stop();

    void waitUntilDone();
};

#endif // RINGBUFFERCONSUMER_H
