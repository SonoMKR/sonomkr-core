#include "iirfilter.h"
#include "biquadfilter.h"

#include <iostream>

IirFilter::IirFilter(Setting& filterConfig)
{
    _nbFilters = filterConfig.lookup("order");
    _filters = new BiquadFilter[_nbFilters];

    for (int i = 0; i < _nbFilters; ++i) {
        Setting& biquad = filterConfig.lookup("filters")[i];
        _filters[i].initialize(biquad);
    }

//    _iirInputSamples.open("/home/valoo/iirInputSamples.dat", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
//    _iirOutputSamples.open("/home/valoo/iirOutputSamples.dat", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
}

IirFilter::~IirFilter()
{
//    _iirInputSamples.close();
//    _iirOutputSamples.close();
}

double IirFilter::filter(double sample)
{
    double ioSample = sample;
//    int16_t intSampleIn(sample * 32768.0);
//    _iirInputSamples.write(reinterpret_cast<const char*>(&intSampleIn), sizeof(intSampleIn));

    for (int i = 0; i < _nbFilters; ++i) {
        ioSample = _filters[i].filter(ioSample);
    }

//    int16_t intSampleOut(ioSample * 32768.0);
//    _iirOutputSamples.write(reinterpret_cast<const char*>(&intSampleOut), sizeof(intSampleOut));

    return ioSample;
}