#include "configuration.h"

Configuration::Configuration(std::string main_file_path, std::string filters_file_path) :
    main_file_path_(main_file_path),
    filters_file_path_(filters_file_path)
{
    filters_config_.readFile(filters_file_path_.c_str());
    main_config_.readFile(main_file_path_.c_str());
    setDefault();
}

void Configuration::readMainConfig(std::string file_path)
{
    main_file_path_ = file_path;
    main_config_.readFile(main_file_path_.c_str());
    setDefault();
}

libconfig::Setting &Configuration::getSetting(std::string path)
{
    libconfig::Setting &setting = main_config_.lookup(path);
    return setting;
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

void Configuration::setDefault()
{
    libconfig::Setting &root = main_config_.getRoot();

    if (!root.exists("autostart"))
    {
        root.add("autostart", libconfig::Setting::TypeBoolean) = false;
    }
    if (!root.exists("controllerBind"))
    {
        root.add("controllerBind", libconfig::Setting::TypeString) = "tcp://*:6666";
    }

    if (!root.exists("audio"))
    {
        root.add("audio", libconfig::Setting::TypeGroup);
    }
    libconfig::Setting &audio = root["audio"];

    if (!audio.exists("soundCard"))
    {
        audio.add("soundCard", libconfig::Setting::TypeString) = "hw:0,0";
    }
    if (!audio.exists("availableChannels"))
    {
        audio.add("availableChannels", libconfig::Setting::TypeInt) = 2;
    }
    if (!audio.exists("sampleRate"))
    {
        audio.add("sampleRate", libconfig::Setting::TypeInt) = 44100;
    }
    if (!audio.exists("bitDepth"))
    {
        audio.add("bitDepth", libconfig::Setting::TypeInt) = 16;
    }
    if (!audio.exists("periods"))
    {
        audio.add("periods", libconfig::Setting::TypeInt) = 4;
    }
    if (!audio.exists("periods"))
    {
        audio.add("periods", libconfig::Setting::TypeInt) = 4096;
    }
    if (!audio.exists("publishBind"))
    {
        audio.add("publishBind", libconfig::Setting::TypeString) = "tcp://*:6660";
    }

    if (!root.exists("channel1"))
    {
        root.add("channel1", libconfig::Setting::TypeGroup);
    }
    libconfig::Setting &channel1 = root["channel1"];

    if (!channel1.exists("active"))
    {
        channel1.add("active", libconfig::Setting::TypeBoolean) = true;
    }
    if (!channel1.exists("strategy"))
    {
        channel1.add("strategy", libconfig::Setting::TypeString) = "G10";
    }
    if (!channel1.exists("fmin"))
    {
        channel1.add("fmin", libconfig::Setting::TypeInt) = 12;
    }
    if (!channel1.exists("fmax"))
    {
        channel1.add("fmin", libconfig::Setting::TypeInt) = 44;
    }
    if (!channel1.exists("integrationPeriod"))
    {
        channel1.add("integrationPeriod", libconfig::Setting::TypeFloat) = 1.0;
    }
    if (!channel1.exists("publishBind"))
    {
        channel1.add("publishBind", libconfig::Setting::TypeString) = "tcp://*:6661";
    }

    if (!root.exists("channel2"))
    {
        root.add("channel2", libconfig::Setting::TypeGroup);
    }
    libconfig::Setting &channel2 = root["channel2"];

    if (!channel2.exists("active"))
    {
        channel2.add("active", libconfig::Setting::TypeBoolean) = true;
    }
    if (!channel1.exists("strategy"))
    {
        channel1.add("strategy", libconfig::Setting::TypeString) = "G10";
    }
    if (!channel2.exists("fmin"))
    {
        channel2.add("fmin", libconfig::Setting::TypeInt) = 12;
    }
    if (!channel2.exists("fmax"))
    {
        channel2.add("fmin", libconfig::Setting::TypeInt) = 44;
    }
    if (!channel2.exists("integrationPeriod"))
    {
        channel2.add("integrationPeriod", libconfig::Setting::TypeFloat) = 1.0;
    }
    if (!channel2.exists("publishBind"))
    {
        channel2.add("publishBind", libconfig::Setting::TypeString) = "tcp://*:6662";
    }

    main_config_.writeFile(main_file_path_.c_str());
}
