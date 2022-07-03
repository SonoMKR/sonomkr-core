#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define AUTOSTART_PATH "autostart"
#define CONTROLLERBIND_PATH "controllerBind"
#define AUDIO_SOUNDCARD_PATH "audio.soundCard"
#define AUDIO_CHANNELS_PATH "audio.availableChannels"
#define AUDIO_SAMPLERATE_PATH "audio.sampleRate"
#define AUDIO_FORMAT_PATH "audio.format"
#define AUDIO_PERIODS_PATH "audio.periods"
#define AUDIO_PERIODSIZE_PATH "audio.periodSize"
#define AUDIO_PUBLISH_PATH "audio.publishBind"
#define CH1_ACTIVE_PATH "channel1.active"
#define CH1_STRATEGY_PATH "channel1.strategy"
#define CH1_FMIN_PATH "channel1.fmin"
#define CH1_FMAX_PATH "channel1.fmax"
#define CH1_INTEGRATION_PATH "channel1.integrationPeriod"
#define CH1_SENSITIVITY_PATH "channel1.sensitivity"
#define CH1_PUBLISH_PATH "channel1.publishBind"
#define CH2_ACTIVE_PATH "channel2.active"
#define CH2_STRATEGY_PATH "channel2.strategy"
#define CH2_FMIN_PATH "channel2.fmin"
#define CH2_FMAX_PATH "channel2.fmax"
#define CH2_INTEGRATION_PATH "channel2.integrationPeriod"
#define CH2_SENSITIVITY_PATH "channel2.sensitivity"
#define CH2_PUBLISH_PATH "channel2.publishBind"

#include <iostream>
#include <libconfig.h++>

using namespace libconfig;

struct AudioConfig
{
  std::string sound_card = "hw:0,0";
  int available_channels = 2;
  int sample_rate = 48000;
  std::string format = "S24_LE";
  int periods = 4;
  int period_size = 4096;
  std::string publish_bind = "ipc:///tmp/sonomkr/audio";
};

struct ChannelConfig
{
  static int channel_count;

  bool active = true;
  std::string strategy = "G10";
  int fmin = 12;
  int fmax = 44;
  float integration_period = 1.0;
  double sensitivity = 0.05; // V/Pa
  std::string publish_bind;
  ChannelConfig() {
    publish_bind = "ipc:///tmp/sonomkr/channel" + std::to_string(channel_count);
    ChannelConfig::channel_count++;
  }
};

class Configuration
{
  private:
    libconfig::Config current_config_;
    libconfig::Config filters_config_;
    std::string main_file_path_;
    std::string filters_file_path_;

    AudioConfig* audio_;
    ChannelConfig* channel_1_;
    ChannelConfig* channel_2_;

    void initConfig(Config &config);

public:
    Configuration(std::string main_file_path, std::string filters_file_path);
    ~Configuration();
    libconfig::Setting &getFilter(int sample_rate, int base, int frequency);
    libconfig::Setting &getAliasingFilter(int sample_rate, int base);

    int loadConfig(std::string file_path);
    int saveCurrentConfig(std::string file_path = "");

    bool autostart_ = true;
    std::string controller_bind_ = "ipc:///tmp/sonomkr/controller";

    inline AudioConfig* getAudioConfig() { return audio_; }
    inline ChannelConfig* getChannel1Config() { return channel_1_; }
    inline ChannelConfig* getChannel2Config() { return channel_2_; }
};

#endif // CONFIGURATION_H
