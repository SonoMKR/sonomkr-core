// Unit test of the A-weighting table (spectrum::AWeight, defines.h).
// Reference: analytic A-weighting of IEC 61672-1, at the exact base-10 mid-band
// frequencies 1000 * 10^((band - 31) / 10) Hz, normalised to 0 dB at 1 kHz.
// The table is rounded to 0.1 dB, as the standard's table of weightings.

#include <cmath>
#include <cstdio>

#include "spectrum.h"

static double aWeightingDb(double f)
{
    const double f2 = f * f;
    const double ra = std::pow(12194.0, 2) * f2 * f2 /
                      ((f2 + std::pow(20.6, 2)) *
                       std::sqrt((f2 + std::pow(107.7, 2)) * (f2 + std::pow(737.9, 2))) *
                       (f2 + std::pow(12194.0, 2)));
    return 20.0 * std::log10(ra);
}

int main()
{
    const int nb_bands = sizeof(spectrum::AWeight) / sizeof(spectrum::AWeight[0]);
    if (nb_bands != FREQ_20kHz + 1)
    {
        std::printf("AWeight has %d values, expected %d (0.8 Hz to 20 kHz)\n", nb_bands, FREQ_20kHz + 1);
        return 1;
    }

    const double offset_1k = aWeightingDb(1000.0);
    int failures = 0;
    for (int band = 0; band < nb_bands; ++band)
    {
        const double f = 1000.0 * std::pow(10.0, (band - FREQ_1kHz) / 10.0);
        const double expected = aWeightingDb(f) - offset_1k;
        const double error = spectrum::AWeight[band] - expected;
        if (std::fabs(error) > 0.051) // table rounded to 0.1 dB
        {
            std::printf("band %2d (%s, %.3f Hz): %.2f dB, expected %.2f dB\n",
                        band, freqToString(band).c_str(), f, spectrum::AWeight[band], expected);
            ++failures;
        }
    }

    if (failures)
    {
        std::printf("%d A-weighting value(s) out of tolerance\n", failures);
        return 1;
    }
    std::printf("all %d A-weighting values match IEC 61672-1\n", nb_bands);
    return 0;
}
