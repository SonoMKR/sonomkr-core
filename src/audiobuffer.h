#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

// #define SINE_TEST
// #define SINE_FREQ 1000.0
// #define SINE_GAIN 1.0
// #define SINE_RATE 44100.0

#include <vector>
#include <math.h>

#include <zmq.hpp>

#include "ringbuffer.h"
#include "configuration.h"

class AudioBuffer {
private:
    Configuration* config_;
    zmq::context_t* zmq_context_;
    zmq::socket_t zmq_pub_socket_;

    unsigned long buffer_size_;
    int nb_channels_;
    std::vector<RingBuffer*> channel_buffers_;

#ifdef SINE_TEST
    double last_time_;
#endif
    double decodeAudioFloat32bit(const unsigned char* input_buffer);
    double decodeAudio32bit(const unsigned char* input_buffer);
    double decodeAudio24bit(const unsigned char* input_buffer);
    double decodeAudio16bit(const unsigned char* input_buffer);

    void pubAudioBuffer(int channel, const double *buffer, const int &buffer_size);

public:
    AudioBuffer(Configuration* config, zmq::context_t* zmq);
    ~AudioBuffer();

    void resetBuffers();
    void writeAudioToBuffers(const unsigned char* input_buffer,
                             const int& size_to_write,
                             int& nb_channels,
                             std::string &format);
    RingBuffer* getChannelBuffer(int channel);
};

#endif // AUDIOBUFFER_H
