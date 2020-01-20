#include "ringbufferconsumer.h"

#include "ringbuffer.h"

RingBufferConsumer::RingBufferConsumer(RingBuffer *buffer, unsigned long size_to_read) :
    buffer_(buffer),
    reader_index_(buffer->registerReader()),
    do_read_(false),
    buffer_ptr_(buffer->getBufferPtr()),
    buffer_size_(buffer->getBufferSize()),
    size_to_read_(size_to_read)
{
}

RingBufferConsumer::~RingBufferConsumer()
{
    do_read_ = false;
    if (read_thread_.joinable()) {
        read_thread_.join();
    }
}

void RingBufferConsumer::run()
{
    //    cout << "begin thread : " << this << endl;
    int size_red = 0;
    unsigned long read_position = 0;

    while (do_read_)
    {
        if (buffer_->waitToBeginRead(reader_index_, size_to_read_, read_position))
        {
            if (!do_read_)
            {
                break;
            }
            size_red = processData(read_position);
            if (size_red < 0)
            {
                continue;
            }
            buffer_->endRead(reader_index_, size_red);
        }
    }
    //    cout << "end thread : " << this << endl;
}

void RingBufferConsumer::start(bool restart)
{
    if (do_read_) {
        if (restart) {
            stop();
        } else {
            return;
        }
    }
    do_read_ = true;
    read_thread_ = std::thread(&RingBufferConsumer::run, this);
}

void RingBufferConsumer::stop()
{
    do_read_ = false;
    if (read_thread_.joinable()) {
        read_thread_.join();
    }
}

void RingBufferConsumer::waitUntilDone()
{
    if (read_thread_.joinable()) {
        read_thread_.join();
    }
}
