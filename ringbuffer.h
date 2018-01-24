#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <iostream>

#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <chrono>

using namespace std;

struct BufferReader {
    ulong readPosition;
    atomic<uint> sizeReadable;

    BufferReader(ulong startPosition) {
        readPosition = startPosition;
        sizeReadable = 0;
    }
};

template <class T>
class RingBuffer {
private:
    shared_timed_mutex _mutex;
    condition_variable_any _notify;
    std::chrono::milliseconds _msTimeout;
    T* _buffer;
    ulong _bufferSize;
    ulong _writePosition;

    vector<BufferReader*> _readersList;

public:
    RingBuffer(const int& size);
    ~RingBuffer();
    int registerReader();
    bool waitToBeginRead(const int& readerIndex, const uint& sizeToRead, ulong& readPosition);
    void endRead(const int& readerIndex, const int& sizeRed);
    void writeToBuffer(const T* inputBuffer, const int& sizeToWrite);
    void resetBuffer();

    inline T* getBufferPtr() {
        return _buffer;
    }
    inline ulong getBufferSize() {
        return _bufferSize;
    }
};

template class RingBuffer<float>;

#endif // RINGBUFFER_H
