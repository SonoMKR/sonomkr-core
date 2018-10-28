#include "audiobuffer.h"

AudioBuffer::AudioBuffer(Configuration *config) : config_(config)
{
#ifdef SINE_TEST
    _lastTime = 0.0;
#endif

    int sample_rate = config_->getSetting(string(AUDIO_SAMPLERATE_PATH));
    buffer_size_ = sample_rate * 30;

    bool is_ch1_active = config_->getSetting(string(CH1_ACTIVE_PATH));
    bool is_ch2_active = config_->getSetting(string(CH2_ACTIVE_PATH));
    nb_channels_ = 0;
    if (is_ch1_active)
    {
        channel_buffers_.push_back(new RingBuffer(buffer_size_));
        nb_channels_++;
    }
    if (is_ch2_active)
    {
        channel_buffers_.push_back(new RingBuffer(buffer_size_));
        nb_channels_++;
    }

    if (!is_ch1_active && !is_ch2_active)
    {
        //no active channels... why ?
    }
}

AudioBuffer::~AudioBuffer()
{
    for (int i = 0; i < channel_buffers_.size(); ++i)
    {
        delete channel_buffers_[i];
    }
}

void AudioBuffer::resetBuffers()
{
    for (int c = 0; c < nb_channels_; ++c)
    {
        channel_buffers_[c]->resetBuffer();
    }
}

RingBuffer *AudioBuffer::getChannelBuffer(int channel)
{
    if (channel > channel_buffers_.size())
    {
        channel = channel_buffers_.size();
    }
    return channel_buffers_[channel];
}

void AudioBuffer::writeAudioToBuffers(const char *input_buffer, const int &buffer_size,
                                      int &nb_channels, int &format_bit)
{
    if (format_bit != 24 && format_bit != 16)
    {
        return; // format not supported
    }
    if (nb_channels_ > nb_channels)
    {
        return; // should not happen
    }

    int sample_byte_size = (format_bit == 24) ? (32 / 8) : (format_bit / 8);
    int frame_byte_size = nb_channels * sample_byte_size;
    int nbSamples = buffer_size / frame_byte_size;

    float audio_buffers[nb_channels_][nbSamples];

    int s = 0;

    for (int i = 0; i < buffer_size; i += frame_byte_size)
    {
        for (int c = 0; c < nb_channels_; ++c)
        {
            float sample = 0.0;
            if (format_bit == 24)
            {
                sample = decodeAudio24bit(&input_buffer[i + c * sample_byte_size]);
            }
            if (format_bit == 16)
            {
                sample = decodeAudio16bit(&input_buffer[i + c * sample_byte_size]);
            }
#ifdef SINE_TEST
            sample = float(SINE_GAIN * sin(2.0 * 3.141592653589793 * float(SINE_FREQ) * _lastTime));
#endif
            audio_buffers[c][s] = sample;
        }

#ifdef SINE_TEST
        _lastTime += 1.0 / float(SINE_RATE);
#endif
        ++s;
    }

    for (int c = 0; c < nb_channels_; ++c)
    {
        channel_buffers_[c]->writeToBuffer(audio_buffers[c], nbSamples);
    }
}

float AudioBuffer::decodeAudio24bit(const char *input_buffer)
{
    int32_t data_sample_32bit = 0;
    float data_sample_sound = 0.0;

    data_sample_32bit = ((input_buffer[0]) | (input_buffer[1] << 8) | (input_buffer[2] << 16));
    if ((data_sample_32bit & (1U << 23)) > 0)
    {                                      //24 bit negative value
        data_sample_32bit |= (0xff << 24); //get 32 bit negative value
    }
    data_sample_sound = (data_sample_32bit / 8388608.0); // / 0.05;

    return data_sample_sound;
}

float AudioBuffer::decodeAudio16bit(const char *input_buffer)
{
    int16_t data_sample_16bit = 0;
    float data_sample_sound = 0.0;

    data_sample_16bit = ((input_buffer[0]) | (input_buffer[1] << 8));
    data_sample_sound = (data_sample_16bit / 32768.0); // / 0.05;

    return data_sample_sound;
}
