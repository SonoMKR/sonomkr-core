#include "biquadfilter.h"

BiquadFilter::BiquadFilter()
{

}

BiquadFilter::BiquadFilter(double coefficients[6]):
    _b0(coefficients[0]),
    _b1(coefficients[1]),
    _b2(coefficients[2]),
    _a1(coefficients[4]),
    _a2(coefficients[5])
{
    _delay1 = 0;
    _delay2 = 0;
}

void BiquadFilter::initialize(double coefficients[6])
{
    _b0 = coefficients[0];
    _b1 = coefficients[1];
    _b2 = coefficients[2];
    _a1 = coefficients[4];
    _a2 = coefficients[5];

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

    outputAcc += inputAcc * _b0;
    outputAcc += _delay1 * _b1;
    outputAcc += _delay2 * _b2;

    _delay2 = _delay1;
    _delay1 = sample;

    return outputAcc;
}
