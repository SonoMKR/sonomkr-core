#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <vector>

using namespace std;

struct BufferReader {
    mutex* readerMutex;
    condition_variable* readerMutexCondition;
    ulong readPosition;
    uint sizeReadable;
};

template <typename T>
class RingBuffer
{
private:
    T* _buffer;
    ulong _bufferSize;
    ulong _writePosition;

    vector<BufferReader> _readersList;

public:
    RingBuffer<T>(const int& size);
    ~RingBuffer<T>();
    int registerReader();
    bool waitToBeginRead(const int &readerIndex, const uint& sizeToRead, ulong &readPosition);
    void endRead(const int& readerIndex, const int &sizeRed);
    void writeToBuffer(const T *inputBuffer, const int &sizeToWrite);
    void resetBuffer();

    inline T* getBufferPtr() {
        return _buffer;
    }
    inline unsigned long getBufferSize() {
        return _bufferSize;
    }
};

#endif // RINGBUFFER_H
