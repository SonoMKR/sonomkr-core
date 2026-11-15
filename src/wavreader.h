#ifndef WAVREADER_H
#define WAVREADER_H

#include <cstdint>
#include <cstddef>
#include <fstream>
#include <string>

// Minimal RIFF/WAVE reader without external dependency.
//
// Supported encodings, mapped to the ALSA format names understood by
// AudioBuffer::writeAudioToBuffers():
//   PCM 16 bit           -> "S16_LE"
//   PCM 24 bit (packed)  -> "S24_3LE"
//   PCM 32 bit           -> "S32_LE"
//   IEEE float 32 bit    -> "FLOAT_LE"
// WAVE_FORMAT_EXTENSIBLE is accepted when its sub-format is PCM or float.
class WavReader
{
  private:
    std::ifstream file_;
    std::string format_;
    int channels_ = 0;
    int sample_rate_ = 0;
    int bytes_per_sample_ = 0;
    int frame_size_ = 0;
    std::streamoff data_offset_ = 0;
    uint64_t frame_count_ = 0;
    uint64_t frame_position_ = 0;

  public:
    // Returns false and fills `error` (if not null) when the file can not be used.
    bool open(const std::string &path, std::string *error = nullptr);
    void close();
    bool isOpen() const { return file_.is_open(); }

    // Reads up to `frames` interleaved frames into `destination`
    // (which must hold frames * frameSize() bytes). Returns the number of frames read.
    size_t readFrames(unsigned char *destination, size_t frames);
    void rewind();

    const std::string &format() const { return format_; }
    int channels() const { return channels_; }
    int sampleRate() const { return sample_rate_; }
    int bytesPerSample() const { return bytes_per_sample_; }
    int frameSize() const { return frame_size_; }
    uint64_t frameCount() const { return frame_count_; }
    double duration() const { return sample_rate_ > 0 ? double(frame_count_) / sample_rate_ : 0.0; }
};

#endif // WAVREADER_H
