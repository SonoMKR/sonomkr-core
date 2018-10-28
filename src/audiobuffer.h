#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

// #define SINE_TEST
// #define SINE_FREQ 1000.0
// #define SINE_GAIN 1.0
// #define SINE_RATE 44100.0

#include <vector>
#include <math.h>

#include "ringbuffer.h"
#include "configuration.h"

class AudioBuffer {
private:
    Configuration* config_;
    unsigned long buffer_size_;
    int nb_channels_;
    std::vector<RingBuffer*> channel_buffers_;

#ifdef SINE_TEST
    double last_time_;
#endif

    float decodeAudio24bit(const char* input_buffer);
    float decodeAudio16bit(const char* input_buffer);

public:
    AudioBuffer(Configuration* config);
    ~AudioBuffer();

    void resetBuffers();
    void writeAudioToBuffers(const char* input_buffer, const int& size_to_write, int& nb_channels,
                             int& format_bit);
    RingBuffer* getChannelBuffer(int channel);
};

#endif // AUDIOBUFFER_H
