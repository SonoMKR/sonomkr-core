#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <map>
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
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
    string toString();
};

namespace spectrum {
    const double AWeight[] {
            A_WEIGHT_0p8Hz,
            A_WEIGHT_1Hz,
            A_WEIGHT_1p25Hz,
            A_WEIGHT_1p6Hz,
            A_WEIGHT_2Hz,
            A_WEIGHT_2p5Hz,
            A_WEIGHT_3p15Hz,
            A_WEIGHT_4Hz,
            A_WEIGHT_5Hz,
            A_WEIGHT_6p3Hz,
            A_WEIGHT_8Hz,
            A_WEIGHT_10Hz,
            A_WEIGHT_12p5Hz,
            A_WEIGHT_16Hz,
            A_WEIGHT_20Hz,
            A_WEIGHT_25Hz,
            A_WEIGHT_31p5Hz,
            A_WEIGHT_40Hz,
            A_WEIGHT_50Hz,
            A_WEIGHT_63Hz,
            A_WEIGHT_80Hz,
            A_WEIGHT_100Hz,
            A_WEIGHT_125Hz,
            A_WEIGHT_160Hz,
            A_WEIGHT_200Hz,
            A_WEIGHT_250Hz,
            A_WEIGHT_315Hz,
            A_WEIGHT_400Hz,
            A_WEIGHT_500Hz,
            A_WEIGHT_630Hz,
            A_WEIGHT_800Hz,
            A_WEIGHT_1kHz,
            A_WEIGHT_1p25kHz,
            A_WEIGHT_1p6kHz,
            A_WEIGHT_2kHz,
            A_WEIGHT_2p5kHz,
            A_WEIGHT_3p15kHz,
            A_WEIGHT_4kHz,
            A_WEIGHT_5kHz,
            A_WEIGHT_6p3kHz,
            A_WEIGHT_8kHz,
            A_WEIGHT_10kHz,
            A_WEIGHT_12p5kHz,
            A_WEIGHT_16kHz,
            A_WEIGHT_20kHz
    };
}

#endif // SPECTRUM_H
