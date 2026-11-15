#ifndef WAVFILESOURCE_H
#define WAVFILESOURCE_H

#include <atomic>
#include <string>
#include <thread>
#include <vector>

#include "audiosource.h"
#include "audiobuffer.h"
#include "configuration.h"
#include "wavreader.h"

// Replaces the ALSA capture by a WAV file, for tests and offline analysis.
//
// The file is pushed into the AudioBuffer period by period, exactly as
// AudioCapture does with the sound card.
//  - speed > 0  : paced playback (1.0 = real time, 10.0 = ten times faster)
//  - speed <= 0 : as fast as possible; the source waits for the spectrum
//                 filters only when they are half a ring buffer behind
//                 (no data is lost)
// When the file has fewer channels than the active spectrum channels,
// its first channel is duplicated.
class WavFileSource : public AudioSource
{
  private:
    Configuration *config_;
    AudioBuffer *audio_buffer_;
    WavReader reader_;
    std::thread play_thread_;
    std::atomic<bool> do_play_;
    std::atomic<bool> finished_;

    std::string path_;
    double speed_;
    bool loop_;
    int period_size_;
    int output_channels_;

    void run();

  public:
    WavFileSource(Configuration *config, AudioBuffer *audio_buffer);
    ~WavFileSource();

    void start(bool restart = false) override;
    void stop() override;
    bool isRunning() const override { return do_play_; }

    // true once the whole file has been pushed (never true when looping)
    bool isFinished() const { return finished_; }

    // Opens the WAV file named in the audio configuration and aligns the
    // sample rate of the configuration on it. Must be called before the
    // AudioBuffer and the spectrum channels are built.
    static bool applyFileFormat(Configuration *config, std::string *error = nullptr);
};

#endif // WAVFILESOURCE_H
