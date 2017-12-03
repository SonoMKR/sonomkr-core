#ifndef BIQUADFILTER_H
#define BIQUADFILTER_H

class BiquadFilter
{
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
    BiquadFilter(double coefficients[6]);
    void initialize(double coefficients[6]);
    void reset();

    double filter(double &sample);
};

#endif // BIQUADFILTER_H
