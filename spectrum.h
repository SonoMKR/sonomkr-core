#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <map>
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>

#include "defines.h"

using namespace std;
using namespace std::chrono;

class Spectrum
{
private:
    int _lowerFreq;
    int _higherFreq;
    int _nbFreq;

    int _validCount;

    system_clock::time_point _time;
    int _milliseconds;

    map<int, double> _leqs;
    map<int, double> _globals;

public:
    Spectrum();
    Spectrum(int lowerFreq, int highFreq, system_clock::time_point time = system_clock::now());
    void reset(int lowerFreq, int highFreq, system_clock::time_point time = system_clock::now());

    void setLeq(int freq, float value);
    void setGlobal(int global, float value);
    void setTime(system_clock::time_point time);

    bool isFull();
    void calculateGlobals();

//    string getHeaderString();
    void toString();
};

#endif // SPECTRUM_H
