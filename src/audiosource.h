#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

// Common interface of everything that feeds an AudioBuffer:
// the ALSA capture (AudioCapture) or a WAV file (WavFileSource).
class AudioSource
{
  public:
    virtual ~AudioSource() {}
    virtual void start(bool restart = false) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

#endif // AUDIOSOURCE_H
