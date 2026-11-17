#ifndef AUDIOCAPTURE_H
#define AUDIOCAPTURE_H

#include <iostream>
#include <atomic>
#include <thread>
#include <string>
#include <alsa/asoundlib.h>

#include "configuration.h"
#include "audiobuffer.h"
#include "audiosource.h"

class AudioCapture : public AudioSource
{
  private:
    Configuration *config_;
    AudioBuffer *audio_buffer_;
    std::thread capture_thread_;
    snd_pcm_t *capture_handle_;
    unsigned char *period_buf_;
    std::string pcm_name_;
    int sample_size_;
    int sample_rate_;
    int frame_size_;
    int channels_;
    int period_size_;
    int periods_;
    std::string format_;
    std::atomic<bool> do_capture_; // read by the capture thread

    snd_pcm_t *open_pcm();
    void run();

  public:
    AudioCapture(Configuration *config, AudioBuffer *audioBuffer);
    ~AudioCapture();
    void start(bool restart = false) override;
    void stop() override;

    inline bool isRunning() const override {
      return do_capture_;
    }
};

#endif // AUDIOCAPTURE_H
