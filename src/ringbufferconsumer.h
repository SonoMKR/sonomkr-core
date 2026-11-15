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
    double *buffer_ptr_;
    unsigned long buffer_size_;
    unsigned long size_to_read_;

    virtual int processData(unsigned long read_position) = 0;
    // called when no block was available before the wait timeout (~10 ms)
    virtual void onIdle() {}
    void run();

  public:
    RingBufferConsumer(RingBuffer *buffer, unsigned long size_to_read);
    virtual ~RingBufferConsumer();
    void start(bool restart = false);
    void stop();

    void waitUntilDone();
    inline bool isRunning() const {
      return do_read_;
    };
};

#endif // RINGBUFFERCONSUMER_H
