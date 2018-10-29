#ifndef IIR_FILTER_H
#define IIR_FILTER_H

#include <array>
#include <libconfig.h++>

#include "biquadfilter.h"

class IirFilter
{
  private:
    int nb_filters_;
    BiquadFilter *filters_;

  public:
    IirFilter(libconfig::Setting &filter_config);
    ~IirFilter();
    double filter(double sample);
};

#endif // IIR_FILTER_H
