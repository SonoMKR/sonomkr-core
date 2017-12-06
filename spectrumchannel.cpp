#include "spectrumchannel.h"
#include "defines.h"

#include "SignalProcessing/sos_coefficients.h"

SpectrumChannel::SpectrumChannel(
        RingBuffer<float> *inputBuffer,
        int sizeToRead,
        int channel,
        int fmin,
        int fmax,
        int sampleRate,
        float integrationPeriod):
    RingBufferConsumer<float>(inputBuffer, sizeToRead),
    _channel(channel),
    _fmin(fmin),
    _fmax(fmax),
    _inputBuffer(inputBuffer),
    _sampleRate(sampleRate),
    _integrationPeriod(integrationPeriod)
{
    _spectrumBufferSize = int(300 / _integrationPeriod); // allways 300 seconds => 5 min
    _spectrumBuffer = new Spectrum[_spectrumBufferSize];
    _spectrumWritePosition = 0;

    _lastTime = system_clock::from_time_t(time(nullptr)); // ignore milliseconds
    _lastMilliseconds = 0;
    _spectrumBuffer[_spectrumWritePosition].reset(_fmin, _fmax, _lastTime);

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
                                _integrationPeriod,
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
                                _integrationPeriod,
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
                                _integrationPeriod,
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
                                _integrationPeriod,
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
    // readPosition is ignored, we only use it to sync on the audio thread
    for (int i = 0; i < _leqs.size(); i++)
    {
        int leqReadPosition;
        bool readyRead = _leqs[i].filter->beginReadLeq(1, leqReadPosition);
        if (!readyRead)
        {
            continue;
        }
        if (leqReadPosition > _spectrumWritePosition)
        {
            // leqRead and sepctrumWrite should be in sync
            // so continue and wait for the current spectrum to be filled
            // will get the next leq value next time
            _leqs[i].filter->endReadLeq(0);
        }
        else
        {
            float* buffer = _leqs[i].filter->getLeqBuffer();
            _spectrumBuffer[_spectrumWritePosition].setLeq(_leqs[i].freq, buffer[leqReadPosition]);
            _leqs[i].filter->endReadLeq(1);

            if (_spectrumBuffer[_spectrumWritePosition].isFull())
            {
                _spectrumBuffer[_spectrumWritePosition].toString();
                _spectrumWritePosition = (_spectrumWritePosition + 1) % _spectrumBufferSize;
                 milliseconds increment(int(_integrationPeriod * 1000));
                _lastTime += increment;
                _spectrumBuffer[_spectrumWritePosition].reset(_fmin, _fmax, _lastTime);
            }

        }
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
