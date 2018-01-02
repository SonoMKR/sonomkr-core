#ifndef BIQUADFILTER_H
#define BIQUADFILTER_H

#include <array>

using namespace std;

class BiquadFilter {
private:
    double _b0;
    double _b1;
    double _b2;
    double _a1;
    double _a2;

    double _delay1;
    double _delay2;

public:
    BiquadFilter();
    BiquadFilter(array<double, 6> coefficients);
    void initialize(array<double, 6> coefficients);
    void reset();

    double filter(double sample);
};

#endif // BIQUADFILTER_H
