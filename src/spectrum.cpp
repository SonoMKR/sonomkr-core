#include "spectrum.h"

Spectrum::Spectrum() :
    nb_freq_(0),
    valid_count_(0)
{
}

Spectrum::Spectrum(int lower_freq, int higher_freq, std::chrono::system_clock::time_point time)
{
    reset(lower_freq, higher_freq, time);
}

void Spectrum::reset(int lower_freq, int higher_freq, std::chrono::system_clock::time_point time)
{
    lower_freq_ = lower_freq;
    higher_freq_ = higher_freq;
    time_ = time;

    nb_freq_ = higher_freq_ - lower_freq_ + 1;
    valid_count_ = 0;

    leqs_.clear();
    globals_.clear();

    for (int freq = lower_freq_; freq <= higher_freq_; freq++)
    {
        leqs_[freq] = -99.0;
    }
    globals_[GLOBAL_LAeq] = -99.0;
}

void Spectrum::setLeq(int freq, float value)
{
    if (leqs_[freq] == -99.0)
    {
        valid_count_++;
    }
    leqs_[freq] = value;
}

void Spectrum::setGlobal(int global, float value)
{
    globals_[global] = value;
}

void Spectrum::setTime(std::chrono::system_clock::time_point time)
{
    time_ = time;
}

bool Spectrum::isFull()
{
    if (valid_count_ >= nb_freq_)
    {
        return true;
    }
    return false;
}

void Spectrum::calculateGlobals()
{
    float tempSumA = 0.0;
    for (int freq = lower_freq_; freq <= higher_freq_; freq++)
    {
        tempSumA += exp10((leqs_[freq] + spectrum::AWeight[freq]) / 10);
    }
    globals_[GLOBAL_LAeq] = 10 * log10(tempSumA);
}

std::string Spectrum::toString()
{
    std::stringstream ss;
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_.time_since_epoch());
    std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);
    std::time_t t = s.count();
    std::size_t milliseconds = ms.count() % 1000;
    ss << std::put_time(localtime(&t), "%F %T") << '.';
    ss << std::setfill('0') << std::setw(3) << milliseconds << ";";
    ss.precision(2);
    ss << std::fixed;
    //    ss << "A" << ":";
    //    ss << globals_[GLOBAL_LAeq] << ";";
    for (int freq = lower_freq_; freq <= higher_freq_; freq++)
    {
        ss << freq << ":";
        ss << leqs_[freq] << ";";
    }
    return ss.str();
}
