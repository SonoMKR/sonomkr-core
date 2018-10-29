#include "iirfilter.h"
#include "biquadfilter.h"

#include <iostream>

IirFilter::IirFilter(libconfig::Setting &filter_config)
{
    nb_filters_ = filter_config.lookup("order");
    filters_ = new BiquadFilter[nb_filters_];

    for (int i = 0; i < nb_filters_; ++i)
    {
        libconfig::Setting &biquad = filter_config.lookup("filters")[i];
        filters_[i].initialize(biquad);
    }
}

IirFilter::~IirFilter()
{
}

double IirFilter::filter(double sample)
{
    double ioSample = sample;

    for (int i = 0; i < nb_filters_; ++i)
    {
        ioSample = filters_[i].filter(ioSample);
    }

    return ioSample;
}
