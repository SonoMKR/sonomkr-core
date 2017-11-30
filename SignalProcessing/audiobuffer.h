#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include "../Shared/ringbuffer.h"

class AudioBuffer : public RingBuffer<float>
{
private:
public:
    AudioBuffer(ulong size);
};

#endif // AUDIOBUFFER_H
