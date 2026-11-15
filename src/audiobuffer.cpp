#include "audiobuffer.h"

AudioBuffer::AudioBuffer(Configuration *config, zmq::context_t* zmq):
    config_(config),
    zmq_context_(zmq),
    zmq_pub_socket_(*zmq, zmq::socket_type::pub)
{
#ifdef SINE_TEST
    _lastTime = 0.0;
#endif

    zmq_pub_socket_.bind(config_->getAudioConfig()->publish_bind.c_str());

    int sample_rate = config_->getAudioConfig()->sample_rate;
    buffer_size_ = sample_rate * 30;

    // channel N always reads input N of the sound card, even when the other channel is inactive
    nb_channels_ = 0;
    if (config_->getChannel1Config()->active)
    {
        channel_buffers_.push_back(new RingBuffer(buffer_size_));
        input_channels_.push_back(0);
        nb_channels_++;
    }
    if (config_->getChannel2Config()->active)
    {
        channel_buffers_.push_back(new RingBuffer(buffer_size_));
        input_channels_.push_back(1);
        nb_channels_++;
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

unsigned long AudioBuffer::maxPendingRead() const
{
    unsigned long pending = 0;
    for (unsigned int c = 0; c < channel_buffers_.size(); ++c)
    {
        unsigned long readable = channel_buffers_[c]->maxPendingRead();
        if (readable > pending)
        {
            pending = readable;
        }
    }
    return pending;
}

RingBuffer *AudioBuffer::getChannelBuffer(int channel)
{
    if (channel > channel_buffers_.size())
    {
        channel = channel_buffers_.size();
    }
    return channel_buffers_[channel];
}

void AudioBuffer::writeAudioToBuffers(const unsigned char *input_buffer, const int &buffer_size,
                                      int &nb_channels, std::string &format)
{
    if (nb_channels_ == 0 || input_channels_.back() >= nb_channels)
    {
        return; // the input does not provide the channel(s) to read
    }
    int sample_byte_size;
    if (format == "S32_LE") {
        sample_byte_size = 32 / 8;
    }
    else if (format == "S24_LE") {
        sample_byte_size = 32 / 8;
    }
    else if (format == "S24_3LE") {
        sample_byte_size = 24 / 8;
    }
    else if (format == "S16_LE") {
        sample_byte_size = 16 / 8;
    }
    else if (format == "FLOAT_LE") {
        sample_byte_size = 32 / 8;
    }
    else {
        std::cerr << ("Unsupported format") << std::endl;
        return; // format not supported
    }

    int frame_byte_size = nb_channels * sample_byte_size;
    int nb_samples = buffer_size / frame_byte_size;

    double audio_buffers[nb_channels_][nb_samples];

    int s = 0;

    for (int i = 0; i < buffer_size; i += frame_byte_size)
    {
        for (int c = 0; c < nb_channels_; ++c)
        {
            const unsigned char *raw = &input_buffer[i + input_channels_[c] * sample_byte_size];
            double sample = 0.0;
            if (format == "S32_LE")
            {
                sample = decodeAudio32bit(raw);
            }
            else if (format == "S24_LE")
            {
                sample = decodeAudio24bit(raw);
            }
            else if (format == "S24_3LE")
            {
                sample = decodeAudio24bit(raw);
            }
            else if (format == "S16_LE")
            {
                sample = decodeAudio16bit(raw);
            }
            else if (format == "FLOAT_LE")
            {
                sample = decodeAudioFloat32bit(raw);
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
        channel_buffers_[c]->writeToBuffer(audio_buffers[c], nb_samples);
        pubAudioBuffer(input_channels_[c], audio_buffers[c], nb_samples);
    }
}
double AudioBuffer::decodeAudioFloat32bit(const unsigned char *input_buffer)
{
    int32_t data_sample_32bit = 0;

    data_sample_32bit = ((input_buffer[0]) | (input_buffer[1] << 8) | (input_buffer[2] << 16) | (input_buffer[3] << 24));

    return reinterpret_cast<float &>(data_sample_32bit);
}

double AudioBuffer::decodeAudio32bit(const unsigned char *input_buffer)
{
    int32_t data_sample_32bit = 0;
    double data_sample_sound = 0.0;

    data_sample_32bit = ((input_buffer[0]) | (input_buffer[1] << 8) | (input_buffer[2] << 16) | (input_buffer[3] << 24));
    data_sample_sound = (data_sample_32bit / 2147483647.0);

    return data_sample_sound;
}

double AudioBuffer::decodeAudio24bit(const unsigned char *input_buffer)
{
    int32_t data_sample_32bit = 0;
    double data_sample_sound = 0.0;

    data_sample_32bit = ((input_buffer[0]) | (input_buffer[1] << 8) | (input_buffer[2] << 16));
    if ((data_sample_32bit & (1U << 23)) > 0)
    {                                      //24 bit negative value
        data_sample_32bit |= (0xff << 24); //get 32 bit negative value
    }
    data_sample_sound = (data_sample_32bit / 8388608.0);

    return data_sample_sound;
}

double AudioBuffer::decodeAudio16bit(const unsigned char *input_buffer)
{
    int16_t data_sample_16bit = 0;
    double data_sample_sound = 0.0;

    data_sample_16bit = (input_buffer[0]) | (input_buffer[1] << 8);
    data_sample_sound = (data_sample_16bit / 32768.0);

    return data_sample_sound;
}

void AudioBuffer::pubAudioBuffer(int channel, const double* buffer, const int &buffer_size)
{
    float_t bufferFloat[buffer_size];
    for (int i = 0; i < buffer_size; i++)
    {
        bufferFloat[i] = buffer[i];
    }
    zmq::message_t msg(buffer_size * sizeof(float_t));
    memcpy(msg.data(), bufferFloat, buffer_size * sizeof(float_t));
    if (channel == 0) {
        zmq_pub_socket_.send(zmq::str_buffer("CH1"), zmq::send_flags::sndmore);
    } else {
        zmq_pub_socket_.send(zmq::str_buffer("CH2"), zmq::send_flags::sndmore);
    }
    zmq_pub_socket_.send(msg, zmq::send_flags::dontwait);
}

