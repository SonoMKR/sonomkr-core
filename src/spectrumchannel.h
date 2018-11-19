#ifndef SPECTRUMCHANNEL_H
#define SPECTRUMCHANNEL_H

#include <vector>
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

    int processData(unsigned long readPosition);

    std::chrono::system_clock::time_point last_time_;
    Spectrum* spectrum_buffer_;
    int spectrum_buffer_size_;
    int spectrum_write_position_;

    zmqpp::context *zmq_context_;
    zmqpp::socket zmq_pub_socket_;

    void newSpectrum(std::string spectrumStr);
    void applyG10Strategy();
    void applyG2Strategy();

public:
    SpectrumChannel(Configuration* config, int channel,
                    RingBuffer* input_buffer, int sizeToRead,
                    zmqpp::context* zmq_context);
    ~SpectrumChannel();
    void run();
    void start();
    void stop();

    inline int getMinFreq() {
        return fmin_;
    }
    inline int getMaxFreq() {
        return fmax_;
    }
    inline bool isActive() {
        return do_read_;
    }
};

#endif // SPECTRUMCHANNEL_H
