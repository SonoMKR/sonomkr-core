#ifndef IIR_FILTER_H
#define IIR_FILTER_H

class BiquadFilter;

class IirFilter
{
private:
    int _nbFilters;
    BiquadFilter* _filters;

public:
    IirFilter(int nbFilters, double sosCoefficients[][6]);
    double filter(double sample);
};

#endif // IIR_FILTER_H
