#ifndef IIR_FILTER_H
#define IIR_FILTER_H

//#include <fstream>
#include <array>
#include <libconfig.h++>

#include "sos_coefficients.h"
#include "libconfig.h++"

class BiquadFilter;

using namespace std;
using namespace libconfig;

class IirFilter {
private:
    int _nbFilters;
    BiquadFilter* _filters;
    //    std::ofstream _iirInputSamples;
    //    std::ofstream _iirOutputSamples;

public:
    IirFilter(Setting& filterConfig);
    ~IirFilter();
    double filter(double sample);
};

#endif // IIR_FILTER_H
