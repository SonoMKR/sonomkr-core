#ifndef BIQUADFILTER_H
#define BIQUADFILTER_H

#include <array>
#include <libconfig.h++>

class BiquadFilter
{
  private:
    double b0;
    double b1_;
    double b2_;
    double a1_;
    double a2_;

    double delay1_;
    double delay2_;

  public:
    BiquadFilter();
    BiquadFilter(libconfig::Setting &biquad_config);
    void initialize(libconfig::Setting &biquad_config);
    void reset();

    double filter(double sample);
};

#endif // BIQUADFILTER_H
