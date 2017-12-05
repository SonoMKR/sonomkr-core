#include "spectrumchannel.h"
#include "defines.h"

#include "SignalProcessing/sos_coefficients.h"

SpectrumChannel::SpectrumChannel(
        RingBuffer<float> *inputBuffer,
        int sizeToRead,
        int channel,
        int fmin,
        int fmax):
    RingBufferConsumer<float>(inputBuffer, sizeToRead),
    _channel(channel),
    _fmin(fmin),
    _fmax(fmax),
    _inputBuffer(inputBuffer)
{
    int sampleRate = 44100;
    float integrationPeriod = 1.0;

    if (_fmin < FREQ_2kHz && (sampleRate % 10 == 0))
    {
        _aliasingFilters.push_back(
                    new AntiAliasingFilter(
                        inputBuffer,
                        _sizeToRead,
                        sampleRate)
                    );
    }
    if (_fmin < FREQ_200Hz && (sampleRate % 100 == 0))
    {
        _aliasingFilters.push_back(
                    new AntiAliasingFilter(
                        _aliasingFilters[0]->getOutputBuffer(),
                        _sizeToRead / 10,
                        sampleRate / 10)
                    );
    }
    if (_fmin < FREQ_20Hz && (sampleRate % 1000 == 0))
    {
        _aliasingFilters.push_back(
                    new AntiAliasingFilter(
                        _aliasingFilters[1]->getOutputBuffer(),
                        _sizeToRead / 100,
                        sampleRate / 100)
                    );
    }

    for (int freq = _fmax; freq >= _fmin; freq--)
    {
        if (freq >= FREQ_2kHz)
        {
            _leqs.push_back(Leq{
                            freq,
                            new LeqFilter(
                                inputBuffer,
                                _sizeToRead,
                                sampleRate,
                                integrationPeriod,
                                NB_SOS,
                                sos::getCoefficients(freq))
                            });
        }
        else if (freq >= FREQ_200Hz && (sampleRate % 10 == 0))
        {
            _leqs.push_back(Leq{
                            freq,
                            new LeqFilter(
                                _aliasingFilters[0]->getOutputBuffer(),
                                _sizeToRead / 10,
                                sampleRate / 10,
                                integrationPeriod,
                                NB_SOS,
                                sos::getCoefficients(freq))
                            });
        }
        else if (freq >= FREQ_20Hz && (sampleRate % 100 == 0))
        {
            _leqs.push_back(Leq{
                            freq,
                            new LeqFilter(
                                _aliasingFilters[1]->getOutputBuffer(),
                                _sizeToRead / 100,
                                sampleRate / 100,
                                integrationPeriod,
                                NB_SOS,
                                sos::getCoefficients(freq))
                            });
        }
        else if (freq >= FREQ_0p8Hz && (sampleRate % 1000 == 0))
        {
            _leqs.push_back(Leq{
                            freq,
                            new LeqFilter(
                                _aliasingFilters[2]->getOutputBuffer(),
                                _sizeToRead / 1000,
                                sampleRate / 1000,
                                integrationPeriod,
                                NB_SOS,
                                sos::getCoefficients(freq))
                            });
        }
    }

}

SpectrumChannel::~SpectrumChannel()
{

}


int SpectrumChannel::processData(ulong readPosition)
{
    bool print = false;
    for (int i = 0; i < _leqs.size(); i++)
    {
        int leqReadPosition;
        if (_leqs[i].filter->beginReadLeq(1, leqReadPosition))
        {
            print = true;
            float* buffer = _leqs[i].filter->getLeqBuffer();
            std::cout << buffer[leqReadPosition];
            _leqs[i].filter->endReadLeq(1);
        }
    }
    if (print)
    {
        std::cout << std::endl;
    }

    return _sizeToRead;
}

void SpectrumChannel::start()
{
    for (int i = 0; i < _aliasingFilters.size(); i++)
    {
        _aliasingFilters[i]->start();
    }
    for (int i = 0; i < _leqs.size(); i++)
    {
        _leqs[i].filter->start();
    }
    RingBufferConsumer<float>::start();
}

void SpectrumChannel::pause()
{
    for (int i = 0; i < _aliasingFilters.size(); i++)
    {
        _aliasingFilters[i]->pause();
    }
    for (int i = 0; i < _leqs.size(); i++)
    {
        _leqs[i].filter->pause();
    }
    RingBufferConsumer<float>::pause();
}
