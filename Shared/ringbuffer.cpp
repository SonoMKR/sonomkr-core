#include "ringbuffer.h"

template<class T>
RingBuffer<T>::RingBuffer(const int &size):
    _bufferSize(size)
{
    _buffer = new T[_bufferSize];
    _writePosition = 0;
}

template<class T>
RingBuffer<T>::~RingBuffer()
{
    delete[] _buffer;
}

template<class T>
int RingBuffer<T>::registerReader()
{
    _readersList.push_back(BufferReader{
        new mutex(),
        new condition_variable(),
        _writePosition,
        0
    });
    return _readersList.size() - 1;
}

template<class T>
bool RingBuffer<T>::waitToBeginRead(const int &readerIndex, const uint &sizeToRead, ulong &readPosition)
{
    uint sizeReadable = _readersList[readerIndex].sizeReadable;

    if (sizeReadable < sizeToRead) {
        unique_lock<mutex> lock(_readersList[readerIndex].readerMutex);
        _readersList[readerIndex].readerMutexCondition->wait(lock);
        lock.unlock();
    }

    readPosition = _readersList[readerIndex].readPosition;
    return true;
}

template<class T>
void RingBuffer<T>::endRead(const int &readerIndex, const int &sizeRed)
{
    _readersList[readerIndex].sizeReadable -= sizeRed;
    _readersList[readerIndex].readPosition = (_readersList[readerIndex].readPosition + sizeRed) % _bufferSize;
}

template<class T>
void RingBuffer<T>::writeToBuffer(const T *inputBuffer, const int &sizeToWrite)
{
    for (int i =0; i < sizeToWrite; ++i)
    {
        _buffer[(i + _writePosition) % _bufferSize] = inputBuffer[i];
    }

    _writePosition = (_writePosition + sizeToWrite) % _bufferSize;

    for(int i = 0; i < _readersList.size(); ++i)
    {
        _readersList[i].sizeReadable += sizeToWrite;
        _readersList[i].readerMutexCondition->notify_one();
    }
}

template<class T>
void RingBuffer<T>::resetBuffer()
{
    _writePosition = 0;

    for(int i = 0; i <  _readersList.size(); ++i)
    {
        _readersList[i].sizeReadable = 0;
        _readersList[i].readPosition = 0;
    }
}
