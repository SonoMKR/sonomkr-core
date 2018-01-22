#include "spectrumchannel.h"
#include "defines.h"

#include "SignalProcessing/sos_coefficients.h"

SpectrumChannel::SpectrumChannel(Configuration* config, int channel,
                                 RingBuffer<float>* inputBuffer, int sizeToRead,
                                 context* zmqContext):
    _config(config),
    _inputBuffer(inputBuffer),
    RingBufferConsumer<float>(inputBuffer, sizeToRead),
    _zmqContext(zmqContext),
    _zmqPubSocket(*_zmqContext, socket_type::publish)
{
    _sampleRate = _config->getSetting(string(AUDIO_SAMPLERATE_PATH));
    string publishBind;
    if (channel == 1) {
        _strategy = _config->getSetting(string(CH1_STRATEGY_PATH)).c_str();
        _fmin = _config->getSetting(string(CH1_FMIN_PATH));
        _fmax = _config->getSetting(string(CH1_FMAX_PATH));
        _integrationPeriod = _config->getSetting(string(CH1_INTEGRATION_PATH));
        publishBind = _config->getSetting(string(CH1_PUBLISH_PATH)).c_str();
    }
    else if (channel == 2) {
        _strategy = _config->getSetting(string(CH2_STRATEGY_PATH)).c_str();
        _fmin = _config->getSetting(string(CH2_FMIN_PATH));
        _fmax = _config->getSetting(string(CH2_FMAX_PATH));
        _integrationPeriod = _config->getSetting(string(CH2_INTEGRATION_PATH));
        publishBind = _config->getSetting(string(CH2_PUBLISH_PATH)).c_str();
    }
    _zmqPubSocket.bind(publishBind.c_str());

    _spectrumBufferSize = int(300 / _integrationPeriod); // allways 300 seconds => 5 min
    _spectrumBuffer = new Spectrum[_spectrumBufferSize];
    _spectrumWritePosition = 0;

    _lastTime = system_clock::from_time_t(time(nullptr)); // ignore milliseconds
    _lastMilliseconds = 0;
    _spectrumBuffer[_spectrumWritePosition].reset(_fmin, _fmax, _lastTime);

    if (_strategy == "G10") {
        applyG10Strategy();
    }
    if (_strategy == "G2") {
        applyG2Strategy();
    }
}

SpectrumChannel::~SpectrumChannel()
{
    _zmqPubSocket.close();
}


int SpectrumChannel::processData(ulong readPosition)
{
    // readPosition is ignored, we only use it to sync on the audio thread
    for (int i = 0; i < _leqs.size(); i++) {
        int leqReadPosition;
        bool readyRead = _leqs[i].filter->beginReadLeq(1, leqReadPosition);
        if (!readyRead) {
            continue;
        }
        if (leqReadPosition != _spectrumWritePosition) {
            // leqRead and sepctrumWrite should be in sync
            // so continue and wait for the current spectrum to be filled
            // will get the next leq value next time
            _leqs[i].filter->endReadLeq(0);
            continue;
        }
        float* buffer = _leqs[i].filter->getLeqBuffer();
        _spectrumBuffer[_spectrumWritePosition].setLeq(_leqs[i].freq, buffer[leqReadPosition]);
        _leqs[i].filter->endReadLeq(1);

        if (_spectrumBuffer[_spectrumWritePosition].isFull()) {
            _spectrumBuffer[_spectrumWritePosition].calculateGlobals();
            newSpectrum(_spectrumBuffer[_spectrumWritePosition].toString());
            _spectrumWritePosition = (_spectrumWritePosition + 1) % _spectrumBufferSize;
            milliseconds increment(int(_integrationPeriod * 1000));
            _lastTime += increment;
            _spectrumBuffer[_spectrumWritePosition].reset(_fmin, _fmax, _lastTime);
        }
    }

    return _sizeToRead;
}

void SpectrumChannel::newSpectrum(string spectrumStr)
{
//    cout << spectrumStr << endl;
    _zmqPubSocket.send(spectrumStr);
}

void SpectrumChannel::start()
{
    for (int i = 0; i < _aliasingFilters.size(); i++) {
        _aliasingFilters[i]->start();
    }
    for (int i = 0; i < _leqs.size(); i++) {
        _leqs[i].filter->start();
    }
    RingBufferConsumer<float>::start();
}

void SpectrumChannel::stop()
{
    for (int i = 0; i < _aliasingFilters.size(); i++) {
        _aliasingFilters[i]->stop();
    }
    for (int i = 0; i < _leqs.size(); i++) {
        _leqs[i].filter->stop();
    }
    RingBufferConsumer<float>::stop();
}

void SpectrumChannel::applyG10Strategy()
{
    if (_fmin < FREQ_2kHz && (_sampleRate % 10 == 0)) {
        _aliasingFilters.push_back(new AntiAliasingFilter(_inputBuffer, _sizeToRead, _sampleRate,
                                   _config->getAliasingFilter(_sampleRate, 10)));
    }
    if (_fmin < FREQ_200Hz && (_sampleRate % 100 == 0)) {
        _aliasingFilters.push_back(new AntiAliasingFilter(_aliasingFilters[0]->getOutputBuffer(),
                                   _sizeToRead / 10, _sampleRate / 10, _config->getAliasingFilter(_sampleRate, 10)));
    }
    if (_fmin < FREQ_20Hz && (_sampleRate % 1000 == 0)) {
        _aliasingFilters.push_back(new AntiAliasingFilter(_aliasingFilters[1]->getOutputBuffer(),
                                   _sizeToRead / 100, _sampleRate / 100, _config->getAliasingFilter(_sampleRate, 10)));
    }

    for (int freq = _fmax; freq >= _fmin; freq--) {
        if (freq >= FREQ_2kHz) {
            _leqs.push_back(Leq{
                freq,
                new LeqFilter(_inputBuffer, _sizeToRead, _sampleRate, _integrationPeriod,
                _config->getFilter(_sampleRate, 10, freq))
            });
        }
        else if (freq >= FREQ_200Hz && (_sampleRate % 10 == 0)) {
            _leqs.push_back(Leq{
                freq,
                new LeqFilter(_aliasingFilters[0]->getOutputBuffer(), _sizeToRead / 10, _sampleRate / 10,
                _integrationPeriod, _config->getFilter(_sampleRate, 10, freq + 10))
            });
        }
        else if (freq >= FREQ_20Hz) {
            if (_sampleRate % 100 == 0) {
                _leqs.push_back(Leq{
                    freq,
                    new LeqFilter(_aliasingFilters[1]->getOutputBuffer(), _sizeToRead / 100, _sampleRate / 100,
                    _integrationPeriod, _config->getFilter(_sampleRate, 10, freq + 20))
                });
            }
            else if (freq >= FREQ_80Hz) {
                _leqs.push_back(Leq{
                    freq,
                    new LeqFilter(_aliasingFilters[0]->getOutputBuffer(), _sizeToRead / 10, _sampleRate / 10,
                    _integrationPeriod, _config->getFilter(_sampleRate, 10, freq + 10))
                });
            }
        }
        else if (freq >= FREQ_0p8Hz) {
            if (_sampleRate % 1000 == 0) {
                _leqs.push_back(Leq{
                    freq,
                    new LeqFilter(_aliasingFilters[2]->getOutputBuffer(), _sizeToRead / 1000, _sampleRate / 1000,
                    _integrationPeriod, _config->getFilter(_sampleRate, 10, freq + 30))
                });
            }
            else if (freq >= FREQ_8Hz) {
                _leqs.push_back(Leq{
                    freq,
                    new LeqFilter(_aliasingFilters[1]->getOutputBuffer(), _sizeToRead / 100, _sampleRate / 100,
                    _integrationPeriod, _config->getFilter(_sampleRate, 10, freq + 20))
                });
            }
        }
    }
}

void SpectrumChannel::applyG2Strategy()
{

}
