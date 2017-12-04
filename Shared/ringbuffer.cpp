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
    _readersList.push_back(new BufferReader(_writePosition));
    return _readersList.size() - 1;
}

template<class T>
bool RingBuffer<T>::waitToBeginRead(const int &readerIndex, const uint &sizeToRead, ulong &readPosition)
{
    if (_readersList[readerIndex]->sizeReadable.load() < sizeToRead) {
        shared_lock<shared_timed_mutex> lock(_mutex);
        _notify.wait(lock);
        lock.unlock();
        if (_readersList[readerIndex]->sizeReadable.load() < sizeToRead) { // handle spurious wakeup
            return false;
        }
    }

    readPosition = _readersList[readerIndex]->readPosition;
    return true;
}

template<class T>
void RingBuffer<T>::endRead(const int &readerIndex, const int &sizeRed)
{
    _readersList[readerIndex]->sizeReadable -= sizeRed;
    _readersList[readerIndex]->readPosition = (_readersList[readerIndex]->readPosition + sizeRed) % _bufferSize;
}

template<class T>
void RingBuffer<T>::writeToBuffer(const T *inputBuffer, const int &sizeToWrite)
{
    for (int i =0; i < sizeToWrite; ++i)
    {
        _buffer[(i + _writePosition) % _bufferSize] = inputBuffer[i];
    }

    _writePosition = (_writePosition + sizeToWrite) % _bufferSize;

    for(uint i = 0; i < _readersList.size(); ++i)
    {
        _readersList[i]->sizeReadable += sizeToWrite;
    }

    _notify.notify_all();
}

template<class T>
void RingBuffer<T>::resetBuffer()
{
    _writePosition = 0;

    for(uint i = 0; i <  _readersList.size(); ++i)
    {
        _readersList[i]->sizeReadable = 0;
        _readersList[i]->readPosition = 0;
    }
}
