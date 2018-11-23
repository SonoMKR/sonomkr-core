#ifndef AUDIOCAPTURE_H
#define AUDIOCAPTURE_H

#include <iostream>
#include <thread>
#include <string>
#include <alsa/asoundlib.h>

#include "configuration.h"
#include "audiobuffer.h"

class AudioCapture
{
  private:
    Configuration *config_;
    AudioBuffer *audio_buffer_;
    std::thread capture_thread_;
    snd_pcm_t *capture_handle_;
    char *period_buf_;
    std::string pcm_name_;
    int sample_size_;
    int sample_rate_;
    int frame_size_;
    int channels_;
    int period_size_;
    int periods_;
    int bit_depth_;
    bool do_capture_;

    snd_pcm_t *open_pcm();
    void run();

  public:
    AudioCapture(Configuration *config, AudioBuffer *audioBuffer);
    ~AudioCapture();
    void start(bool restart = false);
    void stop();

    inline bool isRunning() const {
      return do_capture_;
    }
};

#endif // AUDIOCAPTURE_H
