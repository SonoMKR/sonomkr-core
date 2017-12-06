#ifndef SPECTRUMCHANNEL_H
#define SPECTRUMCHANNEL_H

#include <vector>
#include <chrono>

#include "Dbus/Adaptors/channel.h"
#include "SignalProcessing/audiobuffer.h"
#include "SignalProcessing/leqfilter.h"
#include "SignalProcessing/antialiasingfilter.h"
#include "Shared/ringbuffer.h"
#include "Shared/ringbufferconsumer.h"
#include "spectrum.h"
#include "defines.h"

struct Leq
{
    int freq;
    LeqFilter* filter;
};

using namespace std;
using namespace std::chrono;
using namespace std::chrono;

class SpectrumChannel : public RingBufferConsumer<float>
//        public fr::sonomkr::Channel_adaptor,
{

private:
    int _channel;
    int _fmin, _fmax;
    int _sampleRate;
    float _integrationPeriod;
    RingBuffer<float>* _inputBuffer;
    vector<Leq> _leqs;
    vector<AntiAliasingFilter*> _aliasingFilters;

    int processData(ulong readPosition);

    system_clock::time_point _lastTime;
    int _lastMilliseconds;
    Spectrum* _spectrumBuffer;
    int _spectrumBufferSize;
    int _spectrumWritePosition;

public:
    SpectrumChannel(RingBuffer<float>* inputBuffer,
            int sizeToRead,
            int channel,
            int fmin,
            int fmax,
            int sampleRate,
            float integrationPeriod);
    ~SpectrumChannel();
    void run();
    void start();
    void pause();
};

#endif // SPECTRUMCHANNEL_H
