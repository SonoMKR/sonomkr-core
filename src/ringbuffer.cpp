#include "ringbuffer.h"

RingBuffer::RingBuffer(const unsigned long &size) :
    ms_timeout_(10),
    buffer_size_(size),
    write_position_(0)
{
    buffer_ = new double[buffer_size_];
}

RingBuffer::~RingBuffer()
{
    delete[] buffer_;
}

unsigned long RingBuffer::registerReader()
{
    readers_list_.push_back(new BufferReader(write_position_));
    return readers_list_.size() - 1;
}

bool RingBuffer::waitToBeginRead(const unsigned long &readerIndex, const unsigned long &sizeToRead,
                                 unsigned long &read_position)
{
    if (readers_list_[readerIndex]->size_readable.load() < sizeToRead)
    {
        std::shared_lock<std::shared_timed_mutex> lock(mutex_);
        notify_.wait_until(lock, std::chrono::system_clock::now() + ms_timeout_);
        lock.unlock();
        if (readers_list_[readerIndex]->size_readable.load() < sizeToRead)
        {
            // handle spurious wakeup & timeout
            return false;
        }
    }

    read_position = readers_list_[readerIndex]->read_position;
    return true;
}

void RingBuffer::endRead(const unsigned long &readerIndex, const unsigned int &sizeRed)
{
    readers_list_[readerIndex]->size_readable -= sizeRed;
    readers_list_[readerIndex]->read_position = (readers_list_[readerIndex]->read_position + sizeRed) %
                                                buffer_size_;
}

void RingBuffer::writeToBuffer(const double *inputBuffer, const unsigned int &sizeToWrite)
{
    for (unsigned int i = 0; i < sizeToWrite; ++i)
    {
        buffer_[(i + write_position_) % buffer_size_] = inputBuffer[i];
    }

    write_position_ = (write_position_ + sizeToWrite) % buffer_size_;

    for (unsigned int i = 0; i < readers_list_.size(); ++i)
    {
        readers_list_[i]->size_readable += sizeToWrite;
    }

    notify_.notify_all();
}

void RingBuffer::resetBuffer()
{
    write_position_ = 0;

    for (unsigned int i = 0; i < readers_list_.size(); ++i)
    {
        readers_list_[i]->size_readable = 0;
        readers_list_[i]->read_position = 0;
    }
}
