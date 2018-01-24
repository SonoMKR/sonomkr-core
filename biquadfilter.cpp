#include "biquadfilter.h"

BiquadFilter::BiquadFilter()
{

}

BiquadFilter::BiquadFilter(Setting& biquadConfig)
{
    initialize(biquadConfig);
}

void BiquadFilter::initialize(Setting& biquadConfig)
{
    _b0 = biquadConfig.lookup("b0");
    _b1 = biquadConfig.lookup("b1");
    _b2 = biquadConfig.lookup("b2");
    _a1 = biquadConfig.lookup("a1");
    _a2 = biquadConfig.lookup("a2");

    _delay1 = 0;
    _delay2 = 0;
}

void BiquadFilter::reset()
{
    _delay1 = 0;
    _delay2 = 0;
}

double BiquadFilter::filter(double sample)
{
    double inputAcc = sample;
    double outputAcc = 0;

    inputAcc -= _delay1 * _a1;
    inputAcc -= _delay2 * _a2;

    outputAcc = inputAcc * _b0;
    outputAcc += _delay1 * _b1;
    outputAcc += _delay2 * _b2;

    _delay2 = _delay1;
    _delay1 = inputAcc;

    return outputAcc;
}
