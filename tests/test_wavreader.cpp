// Unit tests of WavReader: header parsing, format mapping, frame reading.
// Test files are written in the current directory (the ctest build directory).

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "wavreader.h"

static int failures = 0;

#define CHECK(cond)                                                                   \
    do                                                                                \
    {                                                                                 \
        if (!(cond))                                                                  \
        {                                                                             \
            std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #cond << std::endl; \
            ++failures;                                                               \
        }                                                                             \
    } while (0)

static void put16(std::vector<unsigned char> &v, uint16_t x)
{
    v.push_back(x & 0xFF);
    v.push_back(x >> 8);
}

static void put32(std::vector<unsigned char> &v, uint32_t x)
{
    for (int i = 0; i < 4; ++i)
        v.push_back((x >> (8 * i)) & 0xFF);
}

static void putTag(std::vector<unsigned char> &v, const char *tag)
{
    v.insert(v.end(), tag, tag + 4);
}

struct WavSpec
{
    uint16_t format_tag;  // 1 = PCM, 3 = float
    uint16_t channels;
    uint32_t rate;
    uint16_t bits;
    bool extensible;
    bool extra_chunk;     // add a LIST chunk (odd size) before data
    bool bogus_data_size; // data size = 0xFFFFFFFF (streamed file)
};

static void writeWav(const std::string &path, const WavSpec &s, const std::vector<unsigned char> &data)
{
    const uint16_t block_align = s.channels * (s.bits / 8);
    std::vector<unsigned char> w;
    putTag(w, "RIFF");
    put32(w, 0); // patched below
    putTag(w, "WAVE");

    putTag(w, "fmt ");
    put32(w, s.extensible ? 40 : 16);
    put16(w, s.extensible ? 0xFFFE : s.format_tag);
    put16(w, s.channels);
    put32(w, s.rate);
    put32(w, s.rate * block_align);
    put16(w, block_align);
    put16(w, s.bits);
    if (s.extensible)
    {
        put16(w, 22);       // cbSize
        put16(w, s.bits);   // valid bits
        put32(w, 0);        // channel mask
        put16(w, s.format_tag); // sub-format GUID, first two bytes
        const unsigned char guid_tail[14] = {0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80,
                                             0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
        w.insert(w.end(), guid_tail, guid_tail + 14);
    }

    if (s.extra_chunk)
    {
        putTag(w, "LIST");
        put32(w, 3);
        w.push_back('a');
        w.push_back('b');
        w.push_back('c');
        w.push_back(0); // pad byte
    }

    putTag(w, "data");
    put32(w, s.bogus_data_size ? 0xFFFFFFFFu : uint32_t(data.size()));
    w.insert(w.end(), data.begin(), data.end());

    const uint32_t riff_size = uint32_t(w.size() - 8);
    std::memcpy(&w[4], &riff_size, 4); // little endian host assumed (x86 / ARM)

    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char *>(w.data()), std::streamsize(w.size()));
}

static void testPcm16Stereo()
{
    std::vector<unsigned char> data;
    for (int i = 0; i < 100; ++i)
    {
        put16(data, uint16_t(int16_t(i)));       // left
        put16(data, uint16_t(int16_t(-i)));      // right
    }
    writeWav("t_pcm16.wav", {1, 2, 48000, 16, false, true, false}, data);

    WavReader r;
    std::string err;
    CHECK(r.open("t_pcm16.wav", &err));
    CHECK(r.format() == "S16_LE");
    CHECK(r.channels() == 2);
    CHECK(r.sampleRate() == 48000);
    CHECK(r.frameSize() == 4);
    CHECK(r.frameCount() == 100);

    std::vector<unsigned char> buf(64 * 4);
    CHECK(r.readFrames(buf.data(), 64) == 64);
    CHECK(int16_t(buf[4 * 10] | (buf[4 * 10 + 1] << 8)) == 10);
    CHECK(int16_t(buf[4 * 10 + 2] | (buf[4 * 10 + 3] << 8)) == -10);
    CHECK(r.readFrames(buf.data(), 64) == 36); // remaining frames only
    CHECK(r.readFrames(buf.data(), 64) == 0);

    r.rewind();
    CHECK(r.readFrames(buf.data(), 1) == 1);
    CHECK(buf[0] == 0 && buf[1] == 0);
}

static void testPcm24()
{
    std::vector<unsigned char> data = {0x01, 0x02, 0x03, 0xFF, 0xFF, 0xFF};
    writeWav("t_pcm24.wav", {1, 1, 44100, 24, false, false, false}, data);
    WavReader r;
    CHECK(r.open("t_pcm24.wav"));
    CHECK(r.format() == "S24_3LE");
    CHECK(r.bytesPerSample() == 3);
    CHECK(r.frameCount() == 2);
}

static void testPcm32AndFloat()
{
    std::vector<unsigned char> data(4 * 10, 0);
    writeWav("t_pcm32.wav", {1, 1, 48000, 32, false, false, false}, data);
    writeWav("t_float.wav", {3, 1, 48000, 32, false, false, false}, data);
    writeWav("t_float_ext.wav", {3, 2, 48000, 32, true, false, false}, data);

    WavReader r;
    CHECK(r.open("t_pcm32.wav"));
    CHECK(r.format() == "S32_LE");
    CHECK(r.open("t_float.wav"));
    CHECK(r.format() == "FLOAT_LE");
    CHECK(r.open("t_float_ext.wav"));
    CHECK(r.format() == "FLOAT_LE");
    CHECK(r.channels() == 2);
    CHECK(r.frameCount() == 5);
}

static void testStreamedDataSize()
{
    std::vector<unsigned char> data(2 * 30, 0);
    writeWav("t_streamed.wav", {1, 1, 48000, 16, false, false, true}, data);
    WavReader r;
    CHECK(r.open("t_streamed.wav"));
    CHECK(r.frameCount() == 30); // clamped to the end of the file
}

static void testRejections()
{
    WavReader r;
    std::string err;
    CHECK(!r.open("does_not_exist.wav", &err));
    CHECK(!err.empty());

    std::vector<unsigned char> data(8, 0);
    writeWav("t_pcm8.wav", {1, 1, 8000, 8, false, false, false}, data);
    CHECK(!r.open("t_pcm8.wav", &err));
    writeWav("t_double.wav", {3, 1, 48000, 64, false, false, false}, data);
    CHECK(!r.open("t_double.wav", &err));

    std::ofstream("t_garbage.wav") << "this is not a wav file";
    CHECK(!r.open("t_garbage.wav", &err));
}

int main()
{
    testPcm16Stereo();
    testPcm24();
    testPcm32AndFloat();
    testStreamedDataSize();
    testRejections();

    for (const char *f : {"t_pcm16.wav", "t_pcm24.wav", "t_pcm32.wav", "t_float.wav", "t_float_ext.wav",
                          "t_streamed.wav", "t_pcm8.wav", "t_double.wav", "t_garbage.wav"})
    {
        std::remove(f);
    }

    if (failures)
    {
        std::cerr << failures << " check(s) failed" << std::endl;
        return 1;
    }
    std::cout << "all WavReader checks passed" << std::endl;
    return 0;
}
