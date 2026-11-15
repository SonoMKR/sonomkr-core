#include <unistd.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <libconfig.h++>
#include <zmq.hpp>

#include "configuration.h"
#include "maincontroller.h"

#include "defines.h"

static void printUsage(const char *program)
{
    std::cout << "Usage: " << program << " [options]\n"
              << "  --config <file>    main configuration (default ./sonomkr.conf, /etc/sonomkr/ as root)\n"
              << "  --filters <file>   filters configuration (default ./filters.conf, /etc/sonomkr/ as root)\n"
              << "  --wav <file>       read audio from a WAV file instead of the sound card\n"
              << "  --wav-speed <x>    playback speed: 1 = real time (default), 0 = as fast as possible\n"
              << "  --wav-loop         restart the WAV file when it ends\n"
              << "  -h, --help         show this help\n";
}

int main(int argc, char *argv[])
{
    std::string cli_main_config;
    std::string cli_filters_config;
    std::string wav_file;
    double wav_speed = 1.0;
    bool wav_loop = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        bool has_value = (i + 1 < argc);
        if (arg == "-h" || arg == "--help")
        {
            printUsage(argv[0]);
            return EXIT_SUCCESS;
        }
        else if (arg == "--config" && has_value)
        {
            cli_main_config = argv[++i];
        }
        else if (arg == "--filters" && has_value)
        {
            cli_filters_config = argv[++i];
        }
        else if (arg == "--wav" && has_value)
        {
            wav_file = argv[++i];
        }
        else if (arg == "--wav-speed" && has_value)
        {
            wav_speed = std::stod(argv[++i]);
        }
        else if (arg == "--wav-loop")
        {
            wav_loop = true;
        }
        else
        {
            std::cerr << "Unknown or incomplete option: " << arg << std::endl;
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    boost::filesystem::path etc_main_config("/etc/sonomkr/sonomkr.conf");
    boost::filesystem::path etc_filters_config("/etc/sonomkr/filters.conf");

    boost::filesystem::create_directory(boost::filesystem::path("/tmp/sonomkr"));

    std::string main_config = "./sonomkr.conf";
    std::string filters_config = "./filters.conf";

    auto is_root = (geteuid() == 0);
    if (is_root) {
        main_config = etc_main_config.generic_string();
        filters_config = etc_filters_config.generic_string();
    }
//    std::cout << is_root << std::endl;
    // std::filesystem::path home = getenv("HOME");

    if (!cli_main_config.empty()) {
        main_config = cli_main_config;
    }
    if (!cli_filters_config.empty()) {
        filters_config = cli_filters_config;
    }

    // Read the file. If there is an error, report it and exit.
    Configuration* config = new Configuration(main_config, filters_config);
    if (!wav_file.empty()) {
        AudioConfig* audio_config = config->getAudioConfig();
        audio_config->wav_file = wav_file;
        audio_config->wav_speed = wav_speed;
        audio_config->wav_loop = wav_loop;
    }
    zmq::context_t context;

    MainController* controller = new MainController(config, &context);
    controller->start();
    controller->waitUntilDone();

    return EXIT_SUCCESS;
}

/*

input samples (48000)[20k - 2kHz]
    x filters
        iir filter
            biquad filter

    aliasing filter & decimation (4800)[1.6kHz - 200Hz]
        x filters
            iir filter
                biquad filter

        aliasing filter & decimation (480)[160Hz - 20Hz]
            x filters
                iir filter
                    biquad filter

            [OPTION] ONLY POSSIBLE IF (fs % 1000 == 0)
            aliasing filter & decimation (48)[16Hz - 0.8Hz]
                x filters
                    iir filter
                        biquad filter

*/
