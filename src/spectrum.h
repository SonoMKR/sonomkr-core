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

class Spectrum
{
private:
    int lower_freq_;
    int higher_freq_;
    int nb_freq_;
    int valid_count_;

    std::chrono::system_clock::time_point time_;
    int milliseconds_;

    std::map<int, double> leqs_;
    std::map<int, double> globals_;

public:
    Spectrum();
    Spectrum(int lower_freq, int higher_freq, std::chrono::system_clock::time_point time = std::chrono::system_clock::now());
    void reset(int lower_freq, int higher_freq, std::chrono::system_clock::time_point time = std::chrono::system_clock::now());

    void setLeq(int freq, float value);
    void setGlobal(int global, float value);
    void setTime(std::chrono::system_clock::time_point time);

    bool isFull();
    void calculateGlobals();

//    string getHeaderString();
    std::string toString();
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
