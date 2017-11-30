#include "audiobuffer.h"

AudioBuffer::AudioBuffer(ulong size):
    RingBuffer<float>(size)
{

}
