#include <unistd.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <libconfig.h++>
#include <zmqpp/zmqpp.hpp>

#include "configuration.h"
#include "maincontroller.h"

#include "defines.h"

int main()
{
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

    // Read the file. If there is an error, report it and exit.
    Configuration* config = new Configuration(main_config, filters_config);
    zmqpp::context context;

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
