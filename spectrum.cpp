#include "spectrum.h"

Spectrum::Spectrum()
{
    _nbFreq = 0;
    _validCount = 0;
}

Spectrum::Spectrum(int lowerFreq, int highFreq, system_clock::time_point time)
{
    reset(lowerFreq, highFreq, time);
}

void Spectrum::reset(int lowerFreq, int highFreq, system_clock::time_point time)
{
    _lowerFreq = lowerFreq;
    _higherFreq = highFreq;
    _time = time;

    _nbFreq = _higherFreq - _lowerFreq + 1;
    _validCount = 0;

    _leqs.clear();
    _globals.clear();

    for (int freq = _lowerFreq; freq <= _higherFreq; freq++) {
        _leqs[freq] = -99.0;
    }
    _globals[GLOBAL_LAeq] = -99.0;
}

void Spectrum::setLeq(int freq, float value)
{
    if (_leqs[freq] == -99.0) {
        _validCount++;
    }
    _leqs[freq] = value;
}

void Spectrum::setGlobal(int global, float value)
{
    _globals[global] = value;
}

void Spectrum::setTime(system_clock::time_point time)
{
    _time = time;
}

bool Spectrum::isFull()
{
    if (_validCount >= _nbFreq) {
        return true;
    }
    return false;
}

void Spectrum::calculateGlobals()
{
    float tempSumA = 0.0;
    for (int freq =  _lowerFreq; freq <= _higherFreq; freq++) {
        tempSumA += exp10((_leqs[freq] + spectrum::AWeight[freq]) / 10);
    }
    _globals[GLOBAL_LAeq] = 10 * log10(tempSumA);
}

string Spectrum::toString()
{
    stringstream ss;
    milliseconds ms = duration_cast<milliseconds>(_time.time_since_epoch());
    seconds s = duration_cast<seconds>(ms);
    std::time_t t = s.count();
    std::size_t milliseconds = ms.count() % 1000;
    ss << put_time(localtime(&t), "%F %T") << '.';
    ss << setfill('0') << setw(3) << milliseconds << ";";
    ss.precision(2);
    ss << fixed;
    ss << "A" << ":";
    ss << _globals[GLOBAL_LAeq] << ";";
    for (int freq = _lowerFreq; freq <= _higherFreq; freq++) {
        ss << freq << ":";
        ss << _leqs[freq] << ";";
    }
    return ss.str();
}
