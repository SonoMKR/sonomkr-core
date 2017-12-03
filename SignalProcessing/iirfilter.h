#ifndef IIR_FILTER_H
#define IIR_FILTER_H

//#include <fstream>

class BiquadFilter;

class IirFilter
{
private:
    int _nbFilters;
    BiquadFilter* _filters;
//    std::ofstream _iirInputSamples;
//    std::ofstream _iirOutputSamples;

public:
    IirFilter(int nbFilters, double sosCoefficients[][6]);
    ~IirFilter();
    double filter(double sample);
};

#endif // IIR_FILTER_H
