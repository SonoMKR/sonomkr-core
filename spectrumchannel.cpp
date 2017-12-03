#include "spectrumchannel.h"

#include "SignalProcessing/sos_coefficients.h"

SpectrumChannel::SpectrumChannel(int channel, int fmin, int fmax, RingBuffer<float> *inputBuffer, int sizeToRead):
    RingBufferConsumer<float>(inputBuffer, sizeToRead),
    _channel(channel),
    _fmin(fmin),
    _fmax(fmax),
    _inputBuffer(inputBuffer)
{
    _leqs.push_back(Leq{FREQ_1kHz, new LeqFilter(_inputBuffer, 1024, 44100, 1, sos::NB_SOS, sos::SOS_1kHz)});
    _leqs.push_back(Leq{FREQ_1p25kHz, new LeqFilter(_inputBuffer, 1024, 44100, 1, sos::NB_SOS, sos::SOS_1p25kHz)});
    _leqs.push_back(Leq{FREQ_1p6kHz, new LeqFilter(_inputBuffer, 1024, 44100, 1, sos::NB_SOS, sos::SOS_1p6kHz)});
    _leqs.push_back(Leq{FREQ_2kHz, new LeqFilter(_inputBuffer, 1024, 44100, 1, sos::NB_SOS, sos::SOS_2kHz)});
}


int SpectrumChannel::processData(ulong readPosition)
{
    for (int i = 0; i < _leqs.size(); i++)
    {
        int leqReadPosition;
        if (_leqs[i].filter->beginReadLeq(1, leqReadPosition))
        {
            float* buffer = _leqs[i].filter->getLeqBuffer();
            std::cout << buffer[leqReadPosition] << std::endl;
            _leqs[i].filter->endReadLeq(1);
        }
    }

    return _sizeToRead;
}

void SpectrumChannel::start()
{
    for (int i = 0; i < _leqs.size(); i++)
    {
        _leqs[i].filter->start();
    }
    RingBufferConsumer<float>::start();
}

void SpectrumChannel::pause()
{
    for (int i = 0; i < _leqs.size(); i++)
    {
        _leqs[i].filter->pause();
    }
    RingBufferConsumer<float>::pause();
}
