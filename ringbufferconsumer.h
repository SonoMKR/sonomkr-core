#ifndef RINGBUFFERCONSUMER_H
#define RINGBUFFERCONSUMER_H

#include <thread>

template<class T>
class RingBuffer;

using namespace std;

template<class T>
class RingBufferConsumer {
protected:
    RingBuffer<T>* _buffer;
    int _readerIndex;
    thread _readThread;
    bool _doRead;
    T* _bufferPtr;
    int _bufferSize;
    int _sizeToRead;
    virtual int processData(ulong readPosition) = 0;

public:
    RingBufferConsumer(RingBuffer<T>* buffer, int sizeToRead);
    virtual ~RingBufferConsumer();
    void run();
    void start();
    void stop();

    void waitUntilDone();
};

template class RingBufferConsumer<float>;

#endif // RINGBUFFERCONSUMER_H
