#include "wavreader.h"

namespace
{
const uint16_t WAVE_FORMAT_PCM = 0x0001;
const uint16_t WAVE_FORMAT_IEEE_FLOAT = 0x0003;
const uint16_t WAVE_FORMAT_EXTENSIBLE = 0xFFFE;

uint16_t readLE16(const unsigned char *p)
{
    return uint16_t(p[0] | (p[1] << 8));
}

uint32_t readLE32(const unsigned char *p)
{
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}

bool fail(std::string *error, const std::string &message)
{
    if (error)
    {
        *error = message;
    }
    return false;
}
} // namespace

bool WavReader::open(const std::string &path, std::string *error)
{
    close();

    file_.open(path, std::ios::binary);
    if (!file_.is_open())
    {
        return fail(error, "unable to open " + path);
    }

    file_.seekg(0, std::ios::end);
    const std::streamoff file_size = file_.tellg();
    file_.seekg(0, std::ios::beg);

    unsigned char header[12];
    if (!file_.read(reinterpret_cast<char *>(header), 12) ||
        std::string(reinterpret_cast<char *>(header), 4) != "RIFF" ||
        std::string(reinterpret_cast<char *>(header + 8), 4) != "WAVE")
    {
        close();
        return fail(error, "not a RIFF/WAVE file: " + path);
    }

    bool fmt_found = false;
    uint16_t audio_format = 0;
    uint16_t bits_per_sample = 0;
    uint16_t block_align = 0;

    // walk through the chunks until "data"
    while (true)
    {
        unsigned char chunk_header[8];
        if (!file_.read(reinterpret_cast<char *>(chunk_header), 8))
        {
            close();
            return fail(error, "no data chunk in " + path);
        }
        const std::string chunk_id(reinterpret_cast<char *>(chunk_header), 4);
        const uint32_t chunk_size = readLE32(chunk_header + 4);
        const std::streamoff chunk_start = file_.tellg();

        if (chunk_id == "fmt ")
        {
            if (chunk_size < 16)
            {
                close();
                return fail(error, "fmt chunk too small");
            }
            unsigned char fmt[40] = {0};
            const uint32_t to_read = chunk_size < sizeof(fmt) ? chunk_size : sizeof(fmt);
            if (!file_.read(reinterpret_cast<char *>(fmt), to_read))
            {
                close();
                return fail(error, "truncated fmt chunk");
            }
            audio_format = readLE16(fmt);
            channels_ = readLE16(fmt + 2);
            sample_rate_ = int(readLE32(fmt + 4));
            block_align = readLE16(fmt + 12);
            bits_per_sample = readLE16(fmt + 14);
            if (audio_format == WAVE_FORMAT_EXTENSIBLE)
            {
                if (chunk_size < 40)
                {
                    close();
                    return fail(error, "extensible fmt chunk too small");
                }
                // first two bytes of the sub-format GUID hold the actual format tag
                audio_format = readLE16(fmt + 24);
            }
            fmt_found = true;
        }
        else if (chunk_id == "data")
        {
            if (!fmt_found)
            {
                close();
                return fail(error, "data chunk found before fmt chunk");
            }
            data_offset_ = chunk_start;
            uint64_t data_size = chunk_size;
            // streamed recorders may leave the size at 0 or 0xFFFFFFFF: clamp to the file
            const uint64_t available = uint64_t(file_size - data_offset_);
            if (data_size == 0 || data_size > available)
            {
                data_size = available;
            }
            if (channels_ <= 0 || block_align == 0)
            {
                close();
                return fail(error, "invalid channel count or block alignment");
            }
            frame_size_ = block_align;
            bytes_per_sample_ = block_align / channels_;
            frame_count_ = data_size / frame_size_;
            break;
        }

        // chunks are word aligned
        file_.seekg(chunk_start + std::streamoff(chunk_size) + std::streamoff(chunk_size & 1), std::ios::beg);
    }

    if (audio_format == WAVE_FORMAT_PCM && bytes_per_sample_ == 2 && bits_per_sample <= 16)
    {
        format_ = "S16_LE";
    }
    else if (audio_format == WAVE_FORMAT_PCM && bytes_per_sample_ == 3)
    {
        format_ = "S24_3LE";
    }
    else if (audio_format == WAVE_FORMAT_PCM && bytes_per_sample_ == 4)
    {
        format_ = "S32_LE";
    }
    else if (audio_format == WAVE_FORMAT_IEEE_FLOAT && bytes_per_sample_ == 4)
    {
        format_ = "FLOAT_LE";
    }
    else
    {
        const std::string details = "format tag " + std::to_string(audio_format) + ", " +
                                    std::to_string(bits_per_sample) + " bits";
        close();
        return fail(error, "unsupported WAV encoding (" + details + ")");
    }

    rewind();
    return true;
}

void WavReader::close()
{
    if (file_.is_open())
    {
        file_.close();
    }
    file_.clear();
    format_.clear();
    channels_ = 0;
    sample_rate_ = 0;
    bytes_per_sample_ = 0;
    frame_size_ = 0;
    data_offset_ = 0;
    frame_count_ = 0;
    frame_position_ = 0;
}

size_t WavReader::readFrames(unsigned char *destination, size_t frames)
{
    if (!file_.is_open() || frame_position_ >= frame_count_)
    {
        return 0;
    }
    const uint64_t remaining = frame_count_ - frame_position_;
    if (frames > remaining)
    {
        frames = size_t(remaining);
    }
    file_.read(reinterpret_cast<char *>(destination), std::streamsize(frames * frame_size_));
    const size_t frames_read = size_t(file_.gcount()) / frame_size_;
    frame_position_ += frames_read;
    return frames_read;
}

void WavReader::rewind()
{
    if (!file_.is_open())
    {
        return;
    }
    file_.clear();
    file_.seekg(data_offset_, std::ios::beg);
    frame_position_ = 0;
}
