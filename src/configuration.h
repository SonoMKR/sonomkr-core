#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define AUTOSTART_PATH "autostart"
#define CONTROLLERBIND_PATH "controllerBind"
#define AUDIO_SOUNDCARD_PATH "audio.soundCard"
#define AUDIO_CHANNELS_PATH "audio.availableChannels"
#define AUDIO_SAMPLERATE_PATH "audio.sample_rate"
#define AUDIO_BITDEPTH_PATH "audio.bitDepth"
#define AUDIO_PERIODS_PATH "audio.periods"
#define AUDIO_PERIODSIZE_PATH "audio.periodSize"
#define AUDIO_PUBLISH_PATH "audio.publishBind"
#define CH1_ACTIVE_PATH "channel1.active"
#define CH1_STRATEGY_PATH "channel1.strategy"
#define CH1_FMIN_PATH "channel1.fmin"
#define CH1_FMAX_PATH "channel1.fmax"
#define CH1_INTEGRATION_PATH "channel1.integrationPeriod"
#define CH1_PUBLISH_PATH "channel1.publishBind"
#define CH2_ACTIVE_PATH "channel2.active"
#define CH2_STRATEGY_PATH "channel2.strategy"
#define CH2_FMIN_PATH "channel2.fmin"
#define CH2_FMAX_PATH "channel2.fmax"
#define CH2_INTEGRATION_PATH "channel2.integrationPeriod"
#define CH2_PUBLISH_PATH "channel2.publishBind"

#include <libconfig.h++>

class Configuration
{
  private:
    libconfig::Config main_config_;
    libconfig::Config filters_config_;
    std::string main_file_path_;
    std::string filters_file_path_;

    void setDefault();

  public:
    Configuration(std::string main_file_path, std::string filters_file_path);
    void readMainConfig(std::string file_path);
    libconfig::Setting &getSetting(std::string path);
    libconfig::Setting &getFilter(int sample_rate, int base, int frequency);
    libconfig::Setting &getAliasingFilter(int sample_rate, int base);
};

#endif // CONFIGURATION_H
