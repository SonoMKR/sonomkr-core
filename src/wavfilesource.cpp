#include "wavfilesource.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>

WavFileSource::WavFileSource(Configuration *config, AudioBuffer *audio_buffer) :
    config_(config),
    audio_buffer_(audio_buffer),
    do_play_(false),
    finished_(false)
{
    AudioConfig *audio_config = config_->getAudioConfig();
    path_ = audio_config->wav_file;
    speed_ = audio_config->wav_speed;
    loop_ = audio_config->wav_loop;
    period_size_ = audio_config->period_size > 0 ? audio_config->period_size : 4096;
    output_channels_ = 0;
}

WavFileSource::~WavFileSource()
{
    stop();
}

bool WavFileSource::applyFileFormat(Configuration *config, std::string *error)
{
    AudioConfig *audio_config = config->getAudioConfig();
    WavReader probe;
    if (!probe.open(audio_config->wav_file, error))
    {
        return false;
    }
    if (probe.sampleRate() != audio_config->sample_rate)
    {
        std::cerr << "WAV sample rate (" << probe.sampleRate() << " Hz) overrides the configured one ("
                  << audio_config->sample_rate << " Hz)" << std::endl;
        audio_config->sample_rate = probe.sampleRate();
    }
    return true;
}

void WavFileSource::run()
{
    std::string error;
    if (!reader_.open(path_, &error))
    {
        std::cerr << "WAV source: " << error << std::endl;
        do_play_ = false;
        return;
    }
    if (reader_.sampleRate() != config_->getAudioConfig()->sample_rate)
    {
        std::cerr << "WAV source: sample rate " << reader_.sampleRate()
                  << " Hz does not match the processing rate "
                  << config_->getAudioConfig()->sample_rate << " Hz" << std::endl;
        do_play_ = false;
        return;
    }

    std::string format = reader_.format();
    const int file_channels = reader_.channels();
    const int sample_size = reader_.bytesPerSample();
    // channel 2 reads the second input: a mono file is duplicated on both inputs
    const int needed_channels = config_->getChannel2Config()->active ? 2 : 1;
    output_channels_ = std::max(file_channels, needed_channels);
    const int output_frame_size = output_channels_ * sample_size;

    std::vector<unsigned char> file_period(size_t(period_size_) * reader_.frameSize());
    std::vector<unsigned char> output_period(size_t(period_size_) * output_frame_size);

    // Max speed mode: the ring buffers do not stop the writer, so unread
    // samples would be overwritten if the filters were more than a buffer
    // behind. Waiting above half a buffer (15 s of audio) prevents any loss
    // without slowing the processing down.
    const unsigned long max_pending = audio_buffer_->getBufferSize() / 2;

    std::cerr << "WAV source: " << path_ << " (" << format << ", " << file_channels << " ch, "
              << reader_.sampleRate() << " Hz, " << reader_.duration() << " s, speed "
              << (speed_ > 0 ? std::to_string(speed_) : std::string("max")) << ")" << std::endl;

    auto next_period = std::chrono::steady_clock::now();

    while (do_play_)
    {
        if (speed_ <= 0)
        {
            while (do_play_ && audio_buffer_->maxPendingRead() > max_pending)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            if (!do_play_)
            {
                break;
            }
        }

        size_t frames = reader_.readFrames(file_period.data(), size_t(period_size_));
        if (frames == 0)
        {
            if (loop_ && reader_.frameCount() > 0)
            {
                reader_.rewind();
                continue;
            }
            finished_ = true;
            std::cerr << "WAV source: end of file" << std::endl;
            break;
        }

        const unsigned char *to_write = file_period.data();
        if (output_channels_ != file_channels)
        {
            for (size_t f = 0; f < frames; ++f)
            {
                const unsigned char *in = &file_period[f * reader_.frameSize()];
                unsigned char *out = &output_period[f * output_frame_size];
                for (int c = 0; c < output_channels_; ++c)
                {
                    const int source_channel = c < file_channels ? c : 0;
                    std::memcpy(out + c * sample_size, in + source_channel * sample_size, sample_size);
                }
            }
            to_write = output_period.data();
        }

        audio_buffer_->writeAudioToBuffers(to_write, int(frames) * output_frame_size,
                                           output_channels_, format);

        if (speed_ > 0)
        {
            next_period += std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::duration<double>(double(frames) / (reader_.sampleRate() * speed_)));
            std::this_thread::sleep_until(next_period);
        }
    }

    reader_.close();
    do_play_ = false;
}

void WavFileSource::start(bool restart)
{
    if (play_thread_.joinable())
    {
        if (do_play_ && !restart)
        {
            return;
        }
        stop();
    }
    finished_ = false;
    do_play_ = true;
    play_thread_ = std::thread(&WavFileSource::run, this);
}

void WavFileSource::stop()
{
    do_play_ = false;
    if (play_thread_.joinable())
    {
        play_thread_.join();
    }
}
