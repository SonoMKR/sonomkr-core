#include "biquadfilter.h"

BiquadFilter::BiquadFilter()
{

}

BiquadFilter::BiquadFilter(libconfig::Setting& biquad_config)
{
    initialize(biquad_config);
}

void BiquadFilter::initialize(libconfig::Setting& biquad_config)
{
    b0 = biquad_config.lookup("b0");
    b1_ = biquad_config.lookup("b1");
    b2_ = biquad_config.lookup("b2");
    a1_ = biquad_config.lookup("a1");
    a2_ = biquad_config.lookup("a2");

    delay1_ = 0;
    delay2_ = 0;
}

void BiquadFilter::reset()
{
    delay1_ = 0;
    delay2_ = 0;
}

double BiquadFilter::filter(double sample)
{
    double input_acc = sample;
    double output_acc = 0;

    input_acc -= delay1_ * a1_;
    input_acc -= delay2_ * a2_;

    output_acc = input_acc * b0;
    output_acc += delay1_ * b1_;
    output_acc += delay2_ * b2_;

    delay2_ = delay1_;
    delay1_ = input_acc;

    return output_acc;
}
