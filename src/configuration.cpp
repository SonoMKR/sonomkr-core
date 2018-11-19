#include "configuration.h"

#include <iostream>

int ChannelConfig::channel_count = 1;

Configuration::Configuration(std::string main_file_path, std::string filters_file_path) :
    main_file_path_(main_file_path),
    filters_file_path_(filters_file_path)
{
    try {
        filters_config_.readFile(filters_file_path_.c_str());
    }
    catch (const libconfig::FileIOException& fioex) {
        std::cerr << "I/O error while reading config file : " << filters_file_path_ << std::endl;
        exit(1);
    }
    catch (const libconfig::ParseException& pex) {
        std::cerr << "Parse config error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        exit(2);
    }
    // main_config_.readFile(main_file_path_.c_str());

    loadConfig(main_file_path_);
}

libconfig::Setting &Configuration::getFilter(int sample_rate, int base, int frequency)
{
    std::string path = "RATE_" + std::to_string(sample_rate) +
                       ".G" + std::to_string(base) +
                       ".FREQ_" + std::to_string(frequency);
    libconfig::Setting &setting = filters_config_.lookup(path);
    return setting;
}

libconfig::Setting &Configuration::getAliasingFilter(int sample_rate, int base)
{
    std::string path = "RATE_" + std::to_string(sample_rate) +
                       ".G" + std::to_string(base) +
                       ".ANTI_ALIASING";
    libconfig::Setting &setting = filters_config_.lookup(path);
    return setting;
}

int Configuration::loadConfig(std::string file_path)
{
    libconfig::Config config;
    try {
        config.readFile(file_path.c_str());
    }
    catch (const libconfig::FileIOException& fioex) {
        std::cerr << "I/O error while reading config file : " << file_path << std::endl;
        return 1;
    }
    catch (const libconfig::ParseException& pex) {
        std::cerr << "Parse config error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return 2;
    }

    config.lookupValue(AUTOSTART_PATH, autostart_);
    config.lookupValue(CONTROLLERBIND_PATH, controller_bind_);

    config.lookupValue(AUDIO_SOUNDCARD_PATH, audio_.sound_card);
    config.lookupValue(AUDIO_CHANNELS_PATH, audio_.available_channels);
    config.lookupValue(AUDIO_SAMPLERATE_PATH, audio_.sample_rate);
    config.lookupValue(AUDIO_BITDEPTH_PATH, audio_.bit_depth);
    config.lookupValue(AUDIO_PERIODS_PATH, audio_.periods);
    config.lookupValue(AUDIO_PERIODSIZE_PATH, audio_.period_size);
    config.lookupValue(AUDIO_PUBLISH_PATH, audio_.publish_bind);

    config.lookupValue(CH1_ACTIVE_PATH, channel_1_.active);
    config.lookupValue(CH1_STRATEGY_PATH, channel_1_.strategy);
    config.lookupValue(CH1_FMIN_PATH, channel_1_.fmin);
    config.lookupValue(CH1_FMAX_PATH, channel_1_.fmax);
    config.lookupValue(CH1_INTEGRATION_PATH, channel_1_.integration_period);
    config.lookupValue(CH1_SENSITIVITY_PATH, channel_1_.sensitivity);
    config.lookupValue(CH1_PUBLISH_PATH, channel_1_.publish_bind);

    config.lookupValue(CH2_ACTIVE_PATH, channel_2_.active);
    config.lookupValue(CH2_STRATEGY_PATH, channel_2_.strategy);
    config.lookupValue(CH2_FMIN_PATH, channel_2_.fmin);
    config.lookupValue(CH2_FMAX_PATH, channel_2_.fmax);
    config.lookupValue(CH2_INTEGRATION_PATH, channel_2_.integration_period);
    config.lookupValue(CH2_SENSITIVITY_PATH, channel_2_.sensitivity);
    config.lookupValue(CH2_PUBLISH_PATH, channel_2_.publish_bind);

    if (!channel_2_.active && ! channel_1_.active)
    {
        std::cerr << "Parse config error : Both channels are inactive" << std::endl;
        return 3;
    }

    return 0;
}