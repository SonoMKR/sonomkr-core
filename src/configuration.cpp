#include "configuration.h"

#include <iostream>

int ChannelConfig::channel_count = 1;

Configuration::Configuration(std::string main_file_path, std::string filters_file_path) :
    main_file_path_(main_file_path),
    filters_file_path_(filters_file_path),
    audio_(new AudioConfig()),
    channel_1_(new ChannelConfig()),
    channel_2_(new ChannelConfig())
{
    try {
        filters_config_.readFile(filters_file_path_.c_str());
    }
    catch (const FileIOException& fioex) {
        std::cerr << "I/O error while reading config file : " << filters_file_path_ << std::endl;
        exit(1);
    }
    catch (const ParseException& pex) {
        std::cerr << "Parse config error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        exit(2);
    }
    // current_config_.readFile(main_file_path_.c_str());

    loadConfig(main_file_path_);
}

Configuration::~Configuration()
{

}

Setting &Configuration::getFilter(int sample_rate, int base, int frequency)
{
    std::string path = "RATE_" + std::to_string(sample_rate) +
                       ".G" + std::to_string(base) +
                       ".FREQ_" + std::to_string(frequency);
    Setting &setting = filters_config_.lookup(path);
    return setting;
}

Setting &Configuration::getAliasingFilter(int sample_rate, int base)
{
    std::string path = "RATE_" + std::to_string(sample_rate) +
                       ".G" + std::to_string(base) +
                       ".ANTI_ALIASING";
    Setting &setting = filters_config_.lookup(path);
    return setting;
}

void Configuration::initConfig(Config& config)
{
    if (!config.lookupValue(AUTOSTART_PATH, autostart_)) {
        Setting& setting = config.getRoot().add(AUTOSTART_PATH, Setting::Type::TypeBoolean);
        setting = autostart_;
    }
    if (!config.lookupValue(CONTROLLERBIND_PATH, controller_bind_)) {
        Setting& setting = config.getRoot().add(CONTROLLERBIND_PATH, Setting::Type::TypeString);
        setting = controller_bind_;
    }

    if (!config.exists("audio")) {
        config.getRoot().add("audio", Setting::Type::TypeGroup);
    }
    Setting& audio_setting = config.lookup("audio");

    if (!config.lookupValue(AUDIO_SOUNDCARD_PATH, audio_->sound_card)) {
        Setting& setting = audio_setting.add("soundCard", Setting::Type::TypeString);
        setting = audio_->sound_card;
    }
    if (!config.lookupValue(AUDIO_CHANNELS_PATH, audio_->available_channels)) {
        Setting& setting = audio_setting.add("availableChannels", Setting::Type::TypeInt);
        setting = audio_->available_channels;
    }
    if (!config.lookupValue(AUDIO_SAMPLERATE_PATH, audio_->sample_rate)) {
        Setting& setting = audio_setting.add("sampleRate", Setting::Type::TypeInt);
        setting = audio_->sample_rate;
    }
    if (!config.lookupValue(AUDIO_BITDEPTH_PATH, audio_->bit_depth)) {
        Setting& setting = audio_setting.add("bitDepth", Setting::Type::TypeInt);
        setting = audio_->bit_depth;
    }
    if (!config.lookupValue(AUDIO_PERIODS_PATH, audio_->periods)) {
        Setting& setting = audio_setting.add("periods", Setting::Type::TypeInt);
        setting = audio_->periods;
    }
    if (!config.lookupValue(AUDIO_PERIODSIZE_PATH, audio_->period_size)) {
        Setting& setting = audio_setting.add("periodSize", Setting::Type::TypeInt);
        setting = audio_->period_size;
    }
    if (!config.lookupValue(AUDIO_PUBLISH_PATH, audio_->publish_bind)) {
        Setting& setting = audio_setting.add("publishBind", Setting::Type::TypeString);
        setting = audio_->publish_bind;
    }

    if (!config.exists("channel1")) {
        config.getRoot().add("channel1", Setting::Type::TypeGroup);
    }
    Setting& ch1_setting = config.lookup("channel1");

    if (!config.lookupValue(CH1_ACTIVE_PATH, channel_1_->active)) {
        Setting& setting = ch1_setting.add("active", Setting::Type::TypeBoolean);
        setting = channel_1_->active;
    }
    if (!config.lookupValue(CH1_STRATEGY_PATH, channel_1_->strategy)) {
        Setting& setting = ch1_setting.add("strategy", Setting::Type::TypeString);
        setting = channel_1_->strategy;
    }
    if (!config.lookupValue(CH1_FMIN_PATH, channel_1_->fmin)) {
        Setting& setting = ch1_setting.add("fmin", Setting::Type::TypeInt);
        setting = channel_1_->fmin;
    }
    if (!config.lookupValue(CH1_FMAX_PATH, channel_1_->fmax)) {
        Setting& setting = ch1_setting.add("fmax", Setting::Type::TypeInt);
        setting = channel_1_->fmax;
    }
    if (!config.lookupValue(CH1_INTEGRATION_PATH, channel_1_->integration_period)) {
        Setting& setting = ch1_setting.add("integrationPeriod", Setting::Type::TypeFloat);
        setting = channel_1_->integration_period;
    }
    if (!config.lookupValue(CH1_SENSITIVITY_PATH, channel_1_->sensitivity)) {
        Setting& setting = ch1_setting.add("sensitivity", Setting::Type::TypeFloat);
        setting = channel_1_->sensitivity;
    }
    if (!config.lookupValue(CH1_PUBLISH_PATH, channel_1_->publish_bind)) {
        Setting& setting = ch1_setting.add("publishBind", Setting::Type::TypeString);
        setting = channel_1_->publish_bind;
    }

    if (!config.exists("channel2")) {
        config.getRoot().add("channel2", Setting::Type::TypeGroup);
    }
    Setting& ch2_setting = config.lookup("channel2");

    if (!config.lookupValue(CH2_ACTIVE_PATH, channel_2_->active)) {
        Setting& setting = ch2_setting.add("active", Setting::Type::TypeBoolean);
        setting = channel_2_->active;
    }
    if (!config.lookupValue(CH2_STRATEGY_PATH, channel_2_->strategy)) {
        Setting& setting = ch2_setting.add("strategy", Setting::Type::TypeString);
        setting = channel_2_->strategy;
    }
    if (!config.lookupValue(CH2_FMIN_PATH, channel_2_->fmin)) {
        Setting& setting = ch2_setting.add("fmin", Setting::Type::TypeInt);
        setting = channel_2_->fmin;
    }
    if (!config.lookupValue(CH2_FMAX_PATH, channel_2_->fmax)) {
        Setting& setting = ch2_setting.add("fmax", Setting::Type::TypeInt);
        setting = channel_2_->fmax;
    }
    if (!config.lookupValue(CH2_INTEGRATION_PATH, channel_2_->integration_period)) {
        Setting& setting = ch2_setting.add("integrationPeriod", Setting::Type::TypeFloat);
        setting = channel_2_->integration_period;
    }
    if (!config.lookupValue(CH2_SENSITIVITY_PATH, channel_2_->sensitivity)) {
        Setting& setting = ch2_setting.add("sensitivity", Setting::Type::TypeFloat);
        setting = channel_2_->sensitivity;
    }
    if (!config.lookupValue(CH2_PUBLISH_PATH, channel_2_->publish_bind)) {
        Setting& setting = ch2_setting.add("publishBind", Setting::Type::TypeString);
        setting = channel_2_->publish_bind;
    }
}

int Configuration::loadConfig(std::string file_path)
{
    Config config;
    try {
        config.readFile(file_path.c_str());
    }
    catch (const FileIOException& fioex) {
        std::cerr << "I/O error while reading config file : " << file_path << std::endl;
        return 1;
    }
    catch (const ParseException& pex) {
        std::cerr << "Parse config error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return 2;
    }

    try {
        initConfig(config);
        saveCurrentConfig(file_path);
    }
    catch (const SettingNameException& ex) {
        std::cerr << ex.what() << "; " << ex.getPath();
    }

    if (!channel_2_->active && ! channel_1_->active) {
        std::cerr << "Parse config error : Both channels are inactive" << std::endl;
        return 3;
    }

    return 0;
}

int Configuration::saveCurrentConfig(std::string file_path)
{
    Config config;
    if (file_path == "") {
        file_path = main_file_path_;
    }

    try {
        initConfig(config);
    }
    catch (const SettingNameException& ex) {
        std::cerr << ex.what() << "; " << ex.getPath();
        return 3;
    }

    try {
        config.writeFile(file_path.c_str());
    }
    catch (const FileIOException& fioex) {
        std::cerr << "I/O error while saving config file : " << file_path << std::endl;
        return 2;
    }
    return 0;
}
