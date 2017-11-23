#include "iirfilter.h"
#include "biquadfilter.h"

IirFilter::IirFilter(int nbFilters, double sosCoefficients[][6]):
    _nbFilters(nbFilters)
{
    _filters = new BiquadFilter[_nbFilters];

    for (int i = 0; i < _nbFilters; ++i) {
        _filters[0].initialize(sosCoefficients[i]);
    }
}

double IirFilter::filter(double sample)
{
    double ioSample = sample;

    for (int i = 0; i < _nbFilters; ++i) {
        ioSample = _filters[0].filter(ioSample);
    }

    return ioSample;
}
