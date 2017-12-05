#ifndef SPECTRUMCHANNEL_H
#define SPECTRUMCHANNEL_H

#include <vector>
#include <iostream>

#include "Dbus/Adaptors/channel.h"
#include "SignalProcessing/audiobuffer.h"
#include "SignalProcessing/leqfilter.h"
#include "SignalProcessing/antialiasingfilter.h"
#include "Shared/ringbuffer.h"
#include "Shared/ringbufferconsumer.h"
#include "defines.h"

struct Leq
{
    int freq;
    LeqFilter* filter;
};

class SpectrumChannel : public RingBufferConsumer<float>
//        public fr::sonomkr::Channel_adaptor,
{

private:
    int _channel;
    int _fmin, _fmax;
    RingBuffer<float>* _inputBuffer;
    vector<Leq> _leqs;
    vector<AntiAliasingFilter*> _aliasingFilters;

    int processData(ulong readPosition);

public:
    SpectrumChannel(
            RingBuffer<float>* inputBuffer,
            int sizeToRead,
            int channel,
            int fmin,
            int fmax);
    ~SpectrumChannel();
    void run();
    void start();
    void pause();
};

#endif // SPECTRUMCHANNEL_H
