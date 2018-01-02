#include "configuration.h"

Configuration::Configuration(string filePath):
    _filePath(filePath)
{
    _config.readFile(_filePath.c_str());
    setDefault();
}

void Configuration::readFile(string filePath)
{
    _filePath = filePath;
    _config.readFile(_filePath.c_str());
    setDefault();
}

Setting& Configuration::getSetting(string path)
{
    Setting& setting = _config.lookup(path);
    return setting;
}

void Configuration::setDefault()
{
    Setting& root = _config.getRoot();

    if (! root.exists("autostart")) {
        root.add("autostart", Setting::TypeBoolean) = false;
    }
    if (! root.exists("controllerBind")) {
        root.add("controllerBind", Setting::TypeString) = "tcp://*:6666";
    }

    if (! root.exists("audio")) {
        root.add("audio", Setting::TypeGroup);
    }
    Setting& audio = root["audio"];

    if (! audio.exists("soundCard")) {
        audio.add("soundCard", Setting::TypeString) = "hw:0,0";
    }
    if (! audio.exists("availableChannels")) {
        audio.add("availableChannels", Setting::TypeInt) = 2;
    }
    if (! audio.exists("sampleRate")) {
        audio.add("sampleRate", Setting::TypeInt) = 44100;
    }
    if (! audio.exists("bitDepth")) {
        audio.add("bitDepth", Setting::TypeInt) = 16;
    }
    if (! audio.exists("periods")) {
        audio.add("periods", Setting::TypeInt) = 4;
    }
    if (! audio.exists("periods")) {
        audio.add("periods", Setting::TypeInt) = 4096;
    }
    if (! audio.exists("publishBind")) {
        audio.add("publishBind", Setting::TypeString) = "tcp://*:6660";
    }

    if (! root.exists("channel1")) {
        root.add("channel1", Setting::TypeGroup);
    }
    Setting& channel1 = root["channel1"];

    if (! channel1.exists("active")) {
        channel1.add("active", Setting::TypeBoolean) = true;
    }
    if (! channel1.exists("fmin")) {
        channel1.add("fmin", Setting::TypeInt) = 12;
    }
    if (! channel1.exists("fmax")) {
        channel1.add("fmin", Setting::TypeInt) = 44;
    }
    if (! channel1.exists("integrationPeriod")) {
        channel1.add("integrationPeriod", Setting::TypeFloat) = 1.0;
    }
    if (! channel1.exists("publishBind")) {
        channel1.add("publishBind", Setting::TypeString) = "tcp://*:6661";
    }

    if (! root.exists("channel2")) {
        root.add("channel2", Setting::TypeGroup);
    }
    Setting& channel2 = root["channel2"];

    if (! channel2.exists("active")) {
        channel2.add("active", Setting::TypeBoolean) = true;
    }
    if (! channel2.exists("fmin")) {
        channel2.add("fmin", Setting::TypeInt) = 12;
    }
    if (! channel2.exists("fmax")) {
        channel2.add("fmin", Setting::TypeInt) = 44;
    }
    if (! channel2.exists("integrationPeriod")) {
        channel2.add("integrationPeriod", Setting::TypeFloat) = 1.0;
    }
    if (! channel2.exists("publishBind")) {
        channel2.add("publishBind", Setting::TypeString) = "tcp://*:6662";
    }

    _config.writeFile(_filePath.c_str());
}
