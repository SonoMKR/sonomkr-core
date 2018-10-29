#ifndef DEFINES_H
#define DEFINES_H

#include <string>

#define MIC_SENSITIVITY 0.05

#define QUANTIFY_SENITIVITY 0.0000215791864375777
#define QUANTIFY_SENITIVITY_16BIT 0.0000215791864375777
#define QUANTIFY_SENITIVITY_24BIT 0.0000000842936969995499

#define P0 0.00002

#define PERIODE_SIZE_2048 2048
#define PERIODE_SIZE_4096 4096
#define PERIODE_SIZE_8192 8192

#define F_RATE_96000Hz 96000
#define F_RATE_51200Hz 51200
#define F_RATE_48000Hz 48000
#define F_RATE_44100Hz 44100

#define GLOBAL_LAeq 0
#define GLOBAL_LZeq 1

#define FREQ_0p8Hz 0
#define FREQ_1Hz 1
#define FREQ_1p25Hz 2
#define FREQ_1p6Hz 3
#define FREQ_2Hz 4
#define FREQ_2p5Hz 5
#define FREQ_3p15Hz 6
#define FREQ_4Hz 7
#define FREQ_5Hz 8
#define FREQ_6p3Hz 9
#define FREQ_8Hz 10
#define FREQ_10Hz 11
#define FREQ_12p5Hz 12
#define FREQ_16Hz 13
#define FREQ_20Hz 14
#define FREQ_25Hz 15
#define FREQ_31p5Hz 16
#define FREQ_40Hz 17
#define FREQ_50Hz 18
#define FREQ_63Hz 19
#define FREQ_80Hz 20
#define FREQ_100Hz 21
#define FREQ_125Hz 22
#define FREQ_160Hz 23
#define FREQ_200Hz 24
#define FREQ_250Hz 25
#define FREQ_315Hz 26
#define FREQ_400Hz 27
#define FREQ_500Hz 28
#define FREQ_630Hz 29
#define FREQ_800Hz 30
#define FREQ_1kHz 31
#define FREQ_1p25kHz 32
#define FREQ_1p6kHz 33
#define FREQ_2kHz 34
#define FREQ_2p5kHz 35
#define FREQ_3p15kHz 36
#define FREQ_4kHz 37
#define FREQ_5kHz 38
#define FREQ_6p3kHz 39
#define FREQ_8kHz 40
#define FREQ_10kHz 41
#define FREQ_12p5kHz 42
#define FREQ_16kHz 43
#define FREQ_20kHz 44

#define FREQ_USER_1 45
#define FREQ_USER_2 46
#define FREQ_USER_3 47
#define FREQ_USER_4 48
#define FREQ_USER_5 49

#define A_WEIGHT_0p8Hz -156.32
#define A_WEIGHT_1Hz -148.58
#define A_WEIGHT_1p25Hz -140.83
#define A_WEIGHT_1p6Hz -132.28
#define A_WEIGHT_2Hz -124.55
#define A_WEIGHT_2p5Hz -116.85
#define A_WEIGHT_3p15Hz -108.89
#define A_WEIGHT_4Hz -93.14
#define A_WEIGHT_5Hz -85.4
#define A_WEIGHT_6p3Hz -85.4
#define A_WEIGHT_8Hz -77.55
#define A_WEIGHT_10Hz -70.43
#define A_WEIGHT_12p5Hz -63.58
#define A_WEIGHT_16Hz -56.42
#define A_WEIGHT_20Hz -50.5
#define A_WEIGHT_25Hz -44.7
#define A_WEIGHT_31p5Hz -39.4
#define A_WEIGHT_40Hz -34.6
#define A_WEIGHT_50Hz -30.2
#define A_WEIGHT_63Hz -26.2
#define A_WEIGHT_80Hz -22.5
#define A_WEIGHT_100Hz -19.1
#define A_WEIGHT_125Hz -16.1
#define A_WEIGHT_160Hz -13.4
#define A_WEIGHT_200Hz -10.9
#define A_WEIGHT_250Hz -8.6
#define A_WEIGHT_315Hz -6.6
#define A_WEIGHT_400Hz -4.8
#define A_WEIGHT_500Hz -3.2
#define A_WEIGHT_630Hz -1.9
#define A_WEIGHT_800Hz -0.8
#define A_WEIGHT_1kHz 0.0
#define A_WEIGHT_1p25kHz 0.6
#define A_WEIGHT_1p6kHz 1.0
#define A_WEIGHT_2kHz 1.2
#define A_WEIGHT_2p5kHz 1.3
#define A_WEIGHT_3p15kHz 1.2
#define A_WEIGHT_4kHz 1.0
#define A_WEIGHT_5kHz 0.5
#define A_WEIGHT_6p3kHz -0.1
#define A_WEIGHT_8kHz -1.1
#define A_WEIGHT_10kHz -2.5
#define A_WEIGHT_12p5kHz -4.3
#define A_WEIGHT_16kHz -6.6
#define A_WEIGHT_20kHz -9.3

inline std::string freqToString(int freq)
{
    switch (freq)
    {
    case 0:
        return std::string("0.8Hz");
    case 1:
        return std::string("1Hz");
    case 2:
        return std::string("1.25Hz");
    case 3:
        return std::string("1.6Hz");
    case 4:
        return std::string("2Hz");
    case 5:
        return std::string("2.5Hz");
    case 6:
        return std::string("3.15Hz");
    case 7:
        return std::string("4Hz");
    case 8:
        return std::string("5Hz");
    case 9:
        return std::string("6.3Hz");
    case 10:
        return std::string("8Hz");
    case 11:
        return std::string("10Hz");
    case 12:
        return std::string("12.5Hz");
    case 13:
        return std::string("16Hz");
    case 14:
        return std::string("20Hz");
    case 15:
        return std::string("25Hz");
    case 16:
        return std::string("31.5Hz");
    case 17:
        return std::string("40Hz");
    case 18:
        return std::string("50Hz");
    case 19:
        return std::string("63Hz");
    case 20:
        return std::string("80Hz");
    case 21:
        return std::string("100Hz");
    case 22:
        return std::string("125Hz");
    case 23:
        return std::string("160Hz");
    case 24:
        return std::string("200Hz");
    case 25:
        return std::string("250Hz");
    case 26:
        return std::string("315Hz");
    case 27:
        return std::string("400Hz");
    case 28:
        return std::string("500Hz");
    case 29:
        return std::string("630Hz");
    case 30:
        return std::string("800Hz");
    case 31:
        return std::string("1kHz");
    case 32:
        return std::string("1.25kHz");
    case 33:
        return std::string("1.6kHz");
    case 34:
        return std::string("2kHz");
    case 35:
        return std::string("2.5kHz");
    case 36:
        return std::string("3.15kHz");
    case 37:
        return std::string("4kHz");
    case 38:
        return std::string("5kHz");
    case 39:
        return std::string("6.3kHz");
    case 40:
        return std::string("8kHz");
    case 41:
        return std::string("10Hz");
    case 42:
        return std::string("12.5kHz");
    case 43:
        return std::string("16kHz");
    case 44:
        return std::string("20kHz");
    case 45:
        return std::string("user_1");
    case 46:
        return std::string("user_2");
    case 47:
        return std::string("user_3");
    case 48:
        return std::string("user_4");
    case 49:
        return std::string("user_5");
    default:
        return std::string("   ");
    }
}

#endif // DEFINES_H
