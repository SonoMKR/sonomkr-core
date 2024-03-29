#include "spectrumchannel.h"

SpectrumChannel::SpectrumChannel(Configuration* config, int channel,
                                 RingBuffer* input_buffer, int size_to_read,
                                 zmq::context_t* zmq_context):
    config_(config),
    input_buffer_(input_buffer),
    RingBufferConsumer(input_buffer, size_to_read),
    zmq_context_(zmq_context),
    zmq_pub_socket_(*zmq_context, zmq::socket_type::pub)
{
    sample_rate_ = config_->getAudioConfig()->sample_rate;

    ChannelConfig* channel_config;
    if (channel == 1)
    {
        channel_config = config_->getChannel1Config();
    }
    else // if (channel == 2)
    {
        channel_config = config_->getChannel2Config();
    }
    strategy_ = channel_config->strategy;
    fmin_ = channel_config->fmin;
    fmax_ = channel_config->fmax;
    integration_period_ = channel_config->integration_period;
    std::string publish_bind = channel_config->publish_bind;
    sentivity_correction_ = - 20 * log10(channel_config->sensitivity * P0);

    zmq_pub_socket_.bind(publish_bind.c_str());

    spectrum_buffer_size_ = int(300 / integration_period_); // allways 300 seconds => 5 min
    spectrum_buffer_ = new Spectrum[spectrum_buffer_size_];
    spectrum_write_position_ = 0;

    last_time_ = std::chrono::system_clock::from_time_t(time(nullptr)); // ignore milliseconds
    spectrum_buffer_[spectrum_write_position_].reset(fmin_, fmax_, last_time_);

    if (strategy_ == "G10")
    {
        applyG10Strategy();
    }
    if (strategy_ == "G2")
    {
        applyG2Strategy();
    }
}

SpectrumChannel::~SpectrumChannel()
{
    for (int i = 0; i < aliasing_filters_.size(); i++)
    {
        aliasing_filters_[i]->stop();
        delete aliasing_filters_[i];
    }
    for (int i = 0; i < leqs_.size(); i++)
    {
        leqs_[i].filter->stop();
        delete leqs_[i].filter;
    }
    delete[] spectrum_buffer_;
    zmq_pub_socket_.close();
}

int SpectrumChannel::processData(unsigned long readPosition)
{
    // readPosition is ignored, we only use it to sync on the audio thread
    for (int i = 0; i < leqs_.size(); i++)
    {
        int leqReadPosition;
        bool readyRead = leqs_[i].filter->beginReadLeq(1, leqReadPosition);
        if (!readyRead)
        {
            continue;
        }
        if (leqReadPosition != spectrum_write_position_)
        {
            // leqRead and sepctrumWrite should be in sync
            // so continue and wait for the current spectrum to be filled
            // will get the next leq value next time
            leqs_[i].filter->endReadLeq(0);
            continue;
        }
        float *buffer = leqs_[i].filter->getLeqBuffer();
        spectrum_buffer_[spectrum_write_position_].setLeq(leqs_[i].freq, buffer[leqReadPosition], sentivity_correction_);
        leqs_[i].filter->endReadLeq(1);

        if (spectrum_buffer_[spectrum_write_position_].isFull())
        {
            spectrum_buffer_[spectrum_write_position_].calculateGlobals();
            emitNewSpectrum(spectrum_buffer_[spectrum_write_position_].toString());
            spectrum_write_position_ = (spectrum_write_position_ + 1) % spectrum_buffer_size_;
            std::chrono::milliseconds increment(int(integration_period_ * 1000));
            last_time_ += increment;
            spectrum_buffer_[spectrum_write_position_].reset(fmin_, fmax_, last_time_);
        }
    }

    return size_to_read_;
}

void SpectrumChannel::emitNewSpectrum(std::string spectrum_str)
{
    // send topic in first part of the message
    zmq::message_t msg(spectrum_str.size());
    zmq_pub_socket_.send(zmq::str_buffer("LEQ"), zmq::send_flags::sndmore);
    zmq_pub_socket_.send(zmq::buffer(spectrum_str), zmq::send_flags::dontwait);
}

void SpectrumChannel::start(bool restart)
{
    if (isRunning())
    {
        stop();
    }
    for (int i = 0; i < aliasing_filters_.size(); i++)
    {
        aliasing_filters_[i]->start(restart);
    }
    for (int i = 0; i < leqs_.size(); i++)
    {
        leqs_[i].filter->start(restart);
    }
    RingBufferConsumer::start(restart);
}

void SpectrumChannel::stop()
{
    for (int i = 0; i < aliasing_filters_.size(); i++)
    {
        aliasing_filters_[i]->stop();
    }
    for (int i = 0; i < leqs_.size(); i++)
    {
        leqs_[i].filter->stop();
    }
    RingBufferConsumer::stop();
}

void SpectrumChannel::applyG10Strategy()
{
    if (fmin_ < FREQ_2kHz && (sample_rate_ % 10 == 0))
    {
        aliasing_filters_.push_back(new AntiAliasingFilter(input_buffer_, size_to_read_, sample_rate_,
                                                          config_->getAliasingFilter(sample_rate_, 10)));
    }
    if (fmin_ < FREQ_200Hz && (sample_rate_ % 100 == 0))
    {
        aliasing_filters_.push_back(new AntiAliasingFilter(aliasing_filters_[0]->getOutputBuffer(),
                                                          size_to_read_ / 10, sample_rate_ / 10, config_->getAliasingFilter(sample_rate_, 10)));
    }
    if (fmin_ < FREQ_20Hz && (sample_rate_ % 1000 == 0))
    {
        aliasing_filters_.push_back(new AntiAliasingFilter(aliasing_filters_[1]->getOutputBuffer(),
                                                          size_to_read_ / 100, sample_rate_ / 100, config_->getAliasingFilter(sample_rate_, 10)));
    }

    for (int freq = fmax_; freq >= fmin_; freq--)
    {
        if (freq >= FREQ_2kHz)
        {
            leqs_.push_back(Leq{
                freq,
                new LeqFilter(input_buffer_, size_to_read_, sample_rate_, integration_period_,
                              config_->getFilter(sample_rate_, 10, freq))});
        }
        else if (freq >= FREQ_200Hz && (sample_rate_ % 10 == 0))
        {
            leqs_.push_back(Leq{
                freq,
                new LeqFilter(aliasing_filters_[0]->getOutputBuffer(), size_to_read_ / 10, sample_rate_ / 10,
                              integration_period_, config_->getFilter(sample_rate_, 10, freq + 10))});
        }
        else if (freq >= FREQ_20Hz)
        {
            if (sample_rate_ % 100 == 0)
            {
                leqs_.push_back(Leq{
                    freq,
                    new LeqFilter(aliasing_filters_[1]->getOutputBuffer(), size_to_read_ / 100, sample_rate_ / 100,
                                  integration_period_, config_->getFilter(sample_rate_, 10, freq + 20))});
            }
            else if (freq >= FREQ_80Hz)
            {
                leqs_.push_back(Leq{
                    freq,
                    new LeqFilter(aliasing_filters_[0]->getOutputBuffer(), size_to_read_ / 10, sample_rate_ / 10,
                                  integration_period_, config_->getFilter(sample_rate_, 10, freq + 10))});
            }
        }
        else if (freq >= FREQ_0p8Hz)
        {
            if (sample_rate_ % 1000 == 0)
            {
                leqs_.push_back(Leq{
                    freq,
                    new LeqFilter(aliasing_filters_[2]->getOutputBuffer(), size_to_read_ / 1000, sample_rate_ / 1000,
                                  integration_period_, config_->getFilter(sample_rate_, 10, freq + 30))});
            }
            else if (freq >= FREQ_8Hz)
            {
                leqs_.push_back(Leq{
                    freq,
                    new LeqFilter(aliasing_filters_[1]->getOutputBuffer(), size_to_read_ / 100, sample_rate_ / 100,
                                  integration_period_, config_->getFilter(sample_rate_, 10, freq + 20))});
            }
        }
    }
}

void SpectrumChannel::applyG2Strategy()
{
    // TODO
}
