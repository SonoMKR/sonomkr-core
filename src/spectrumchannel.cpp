#include "spectrumchannel.h"

SpectrumChannel::SpectrumChannel(Configuration* config, int channel,
                                 RingBuffer* input_buffer, int size_to_read,
                                 zmqpp::context* zmq_context):
    config_(config),
    input_buffer_(input_buffer),
    RingBufferConsumer(input_buffer, size_to_read),
    zmq_context_(zmq_context),
    zmq_pub_socket_(*zmq_context, zmqpp::socket_type::publish)
{
    sample_rate_ = config_->getSetting(std::string(AUDIO_SAMPLERATE_PATH));
    std::string publish_bind;
    if (channel == 1)
    {
        strategy_ = config_->getSetting(std::string(CH1_STRATEGY_PATH)).c_str();
        fmin_ = config_->getSetting(std::string(CH1_FMIN_PATH));
        fmax_ = config_->getSetting(std::string(CH1_FMAX_PATH));
        integration_period_ = config_->getSetting(std::string(CH1_INTEGRATION_PATH));
        publish_bind = config_->getSetting(std::string(CH1_PUBLISH_PATH)).c_str();
    }
    else if (channel == 2)
    {
        strategy_ = config_->getSetting(std::string(CH2_STRATEGY_PATH)).c_str();
        fmin_ = config_->getSetting(std::string(CH2_FMIN_PATH));
        fmax_ = config_->getSetting(std::string(CH2_FMAX_PATH));
        integration_period_ = config_->getSetting(std::string(CH2_INTEGRATION_PATH));
        publish_bind = config_->getSetting(std::string(CH2_PUBLISH_PATH)).c_str();
    }
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
        spectrum_buffer_[spectrum_write_position_].setLeq(leqs_[i].freq, buffer[leqReadPosition]);
        leqs_[i].filter->endReadLeq(1);

        if (spectrum_buffer_[spectrum_write_position_].isFull())
        {
            spectrum_buffer_[spectrum_write_position_].calculateGlobals();
            newSpectrum(spectrum_buffer_[spectrum_write_position_].toString());
            spectrum_write_position_ = (spectrum_write_position_ + 1) % spectrum_buffer_size_;
            std::chrono::milliseconds increment(int(integration_period_ * 1000));
            last_time_ += increment;
            spectrum_buffer_[spectrum_write_position_].reset(fmin_, fmax_, last_time_);
        }
    }

    return size_to_read_;
}

void SpectrumChannel::newSpectrum(std::string spectrum_str)
{
    // send topic in first part of the message
    zmqpp::message msg;
    msg.add("LEQ");
    msg.add(spectrum_str);
    zmq_pub_socket_.send(msg);
}

void SpectrumChannel::start()
{
    for (int i = 0; i < _aliasingFilters.size(); i++)
    {
        _aliasingFilters[i]->start();
    }
    for (int i = 0; i < leqs_.size(); i++)
    {
        leqs_[i].filter->start();
    }
    RingBufferConsumer::start();
}

void SpectrumChannel::stop()
{
    for (int i = 0; i < _aliasingFilters.size(); i++)
    {
        _aliasingFilters[i]->stop();
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
        _aliasingFilters.push_back(new AntiAliasingFilter(input_buffer_, size_to_read_, sample_rate_,
                                                          config_->getAliasingFilter(sample_rate_, 10)));
    }
    if (fmin_ < FREQ_200Hz && (sample_rate_ % 100 == 0))
    {
        _aliasingFilters.push_back(new AntiAliasingFilter(_aliasingFilters[0]->getOutputBuffer(),
                                                          size_to_read_ / 10, sample_rate_ / 10, config_->getAliasingFilter(sample_rate_, 10)));
    }
    if (fmin_ < FREQ_20Hz && (sample_rate_ % 1000 == 0))
    {
        _aliasingFilters.push_back(new AntiAliasingFilter(_aliasingFilters[1]->getOutputBuffer(),
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
                new LeqFilter(_aliasingFilters[0]->getOutputBuffer(), size_to_read_ / 10, sample_rate_ / 10,
                              integration_period_, config_->getFilter(sample_rate_, 10, freq + 10))});
        }
        else if (freq >= FREQ_20Hz)
        {
            if (sample_rate_ % 100 == 0)
            {
                leqs_.push_back(Leq{
                    freq,
                    new LeqFilter(_aliasingFilters[1]->getOutputBuffer(), size_to_read_ / 100, sample_rate_ / 100,
                                  integration_period_, config_->getFilter(sample_rate_, 10, freq + 20))});
            }
            else if (freq >= FREQ_80Hz)
            {
                leqs_.push_back(Leq{
                    freq,
                    new LeqFilter(_aliasingFilters[0]->getOutputBuffer(), size_to_read_ / 10, sample_rate_ / 10,
                                  integration_period_, config_->getFilter(sample_rate_, 10, freq + 10))});
            }
        }
        else if (freq >= FREQ_0p8Hz)
        {
            if (sample_rate_ % 1000 == 0)
            {
                leqs_.push_back(Leq{
                    freq,
                    new LeqFilter(_aliasingFilters[2]->getOutputBuffer(), size_to_read_ / 1000, sample_rate_ / 1000,
                                  integration_period_, config_->getFilter(sample_rate_, 10, freq + 30))});
            }
            else if (freq >= FREQ_8Hz)
            {
                leqs_.push_back(Leq{
                    freq,
                    new LeqFilter(_aliasingFilters[1]->getOutputBuffer(), size_to_read_ / 100, sample_rate_ / 100,
                                  integration_period_, config_->getFilter(sample_rate_, 10, freq + 20))});
            }
        }
    }
}

void SpectrumChannel::applyG2Strategy()
{
    // TODO
}
