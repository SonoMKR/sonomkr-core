#ifndef IIR_FILTER_H
#define IIR_FILTER_H

//#include <fstream>
#include <array>

#include "sos_coefficients.h"

class BiquadFilter;

using namespace std;

class IirFilter
{
private:
    int _nbFilters;
    BiquadFilter* _filters;
//    std::ofstream _iirInputSamples;
//    std::ofstream _iirOutputSamples;

public:
    IirFilter(int nbFilters,
              array<array<double,6>, NB_SOS> sosCoefficients);
    ~IirFilter();
    double filter(double sample);
};

#endif // IIR_FILTER_H
