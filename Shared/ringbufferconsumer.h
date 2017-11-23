#ifndef RINGBUFFERCONSUMER_H
#define RINGBUFFERCONSUMER_H

#include <thread>

template<typename T>
class RingBuffer;

using namespace std;

template<typename T>
class RingBufferConsumer
{
private:
    RingBuffer<T>* _buffer;
    int _readerIndex;
    bool _doRead;
    thread _readThread;

protected:
    T* _bufferPtr;
    int _bufferSize;
    int _sizeToRead;
    virtual int processData(ulong readPosition) = 0;

public:
    RingBufferConsumer<T>(RingBuffer<T>* buffer, int sizeToRead);
    virtual ~RingBufferConsumer<T>();
    void run();
    void start();
    void pause();
};

#endif // RINGBUFFERCONSUMER_H
