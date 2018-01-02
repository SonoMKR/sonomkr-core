#ifndef SOS_COEFFICIENTS_H
#define SOS_COEFFICIENTS_H
// FILTER COEFFICIENTS DESIGNED IN PYTHON

#define NB_SOS 10

#include <array>
#include "../defines.h"

using namespace std;

namespace sos {


    extern array<array<double,6>,NB_SOS> SOS_0p8kHz;
    extern array<array<double,6>,NB_SOS> SOS_1kHz;
    extern array<array<double,6>,NB_SOS> SOS_1p25kHz;
    extern array<array<double,6>,NB_SOS> SOS_1p6kHz;
    extern array<array<double,6>,NB_SOS> SOS_2kHz;
    extern array<array<double,6>,NB_SOS> SOS_2p5kHz;
    extern array<array<double,6>,NB_SOS> SOS_3p15kHz;
    extern array<array<double,6>,NB_SOS> SOS_4kHz;
    extern array<array<double,6>,NB_SOS> SOS_5kHz;
    extern array<array<double,6>,NB_SOS> SOS_6p3kHz;
    extern array<array<double,6>,NB_SOS> SOS_8kHz;
    extern array<array<double,6>,NB_SOS> SOS_10kHz;
    extern array<array<double,6>,NB_SOS> SOS_12p5kHz;
    extern array<array<double,6>,NB_SOS> SOS_16kHz;
    extern array<array<double,6>,NB_SOS> SOS_20kHz;
    extern array<array<double,6>,NB_SOS> SOS_ALIASING;

    extern array<array<double,6>,NB_SOS> getCoefficients(int decimateRatio, int freq);

}
#endif // SOS_COEFFICIENTS_H
