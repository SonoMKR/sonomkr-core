#ifndef SPECTRUMCHANNEL_H
#define SPECTRUMCHANNEL_H

#include <vector>
#include <atomic>
#include <string>
#include <chrono>
#include <zmqpp/zmqpp.hpp>

#include "configuration.h"
#include "audiobuffer.h"
#include "leqfilter.h"
#include "antialiasingfilter.h"
#include "ringbuffer.h"
#include "ringbufferconsumer.h"
#include "spectrum.h"
#include "defines.h"

struct Leq {
    int freq;
    LeqFilter* filter;
};

class SpectrumChannel : public RingBufferConsumer
{
private:
    Configuration* config_;
    std::string strategy_;
    int fmin_, fmax_;
    int sample_rate_;
    float integration_period_;
    RingBuffer* input_buffer_;
    std::vector<Leq> leqs_;
    std::vector<AntiAliasingFilter*> aliasing_filters_;

    std::chrono::system_clock::time_point last_time_;
    Spectrum* spectrum_buffer_;
    int spectrum_buffer_size_;
    int spectrum_write_position_;

    std::atomic<float> sentivity_correction_;

    zmqpp::context *zmq_context_;
    zmqpp::socket zmq_pub_socket_;

    void applyG10Strategy();
    void applyG2Strategy();
    int processData(unsigned long readPosition);

    void emitNewSpectrum(std::string spectrumStr);

public:
    SpectrumChannel(Configuration* config, int channel,
                    RingBuffer* input_buffer, int sizeToRead,
                    zmqpp::context* zmq_context);
    ~SpectrumChannel();
    void run();
    void start(bool restart = false);
    void stop();

    inline void setSentivityCorrection(float value)
    {
        sentivity_correction_.store(value);
    }

    inline int getMinFreq() const {
        return fmin_;
    }
    inline int getMaxFreq() const {
        return fmax_;
    }
};

#endif // SPECTRUMCHANNEL_H
