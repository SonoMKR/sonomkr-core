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

    for (int freq = _lowerFreq; freq <= _higherFreq; freq++)
    {
        _leqs[freq] = -99.0;
    }
    _globals[GLOBAL_LAeq] = -99.0;
}

void Spectrum::setLeq(int freq, float value)
{
    if (_leqs[freq] == -99.0)
    {
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
    if (_validCount >= _nbFreq)
    {
        return true;
    }
    return false;
}

void Spectrum::calculateGlobals()
{

}

void Spectrum::toString()
{
    milliseconds ms = duration_cast<milliseconds>(_time.time_since_epoch());
    seconds s = duration_cast<seconds>(ms);
    std::time_t t = s.count();
    std::size_t milliseconds = ms.count() % 1000;
    cout << put_time(localtime(&t), "%F %T") << '.';
    cout << setfill('0') << setw(3) << milliseconds << ";";
    cout.precision(2);
    cout << fixed;
    cout << _globals[GLOBAL_LAeq] << ";";
    for (int freq = _lowerFreq; freq < _higherFreq; freq++)
    {
        cout << _leqs[freq] << ";";
    }
    cout << endl;
}
