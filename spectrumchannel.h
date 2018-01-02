#ifndef SPECTRUMCHANNEL_H
#define SPECTRUMCHANNEL_H

#include <vector>
#include <string>
#include <chrono>
#include <zmqpp/zmqpp.hpp>

#include "configuration.h"
#include "SignalProcessing/audiobuffer.h"
#include "SignalProcessing/leqfilter.h"
#include "SignalProcessing/antialiasingfilter.h"
#include "Shared/ringbuffer.h"
#include "Shared/ringbufferconsumer.h"
#include "spectrum.h"
#include "defines.h"

struct Leq {
    int freq;
    LeqFilter* filter;
};

using namespace std;
using namespace std::chrono;
using namespace zmqpp;

class SpectrumChannel : public RingBufferConsumer<float> {
private:
    Configuration* _config;
    context* _zmqContext;
    socket _zmqPubSocket;
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

    void newSpectrum(string spectrumStr);

public:
    SpectrumChannel(Configuration* config, int channel,
                    RingBuffer<float>* inputBuffer, int sizeToRead,
                    context* zmqContext);
    ~SpectrumChannel();
    void run();
    void start();
    void stop();

    int getMinFreq() {
        return _fmin;
    }
    int getMaxFreq() {
        return _fmax;
    }
    bool isActive() {
        return _doRead;
    }
};

#endif // SPECTRUMCHANNEL_H
