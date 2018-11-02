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

    // std::filesystem::path home = getenv("HOME");

    std::string main_config = "./sonomkr.conf";
    std::string filters_config = "./filters.conf";

    // Read the file. If there is an error, report it and exit.
    try {
        Configuration config(main_config, filters_config);
        zmqpp::context context;

        MainController* controller = new MainController(&config, &context);
        controller->start();
        controller->waitUntilDone();
        return EXIT_SUCCESS;
    }
    catch (const libconfig::FileIOException& fioex) {
        std::cerr << "I/O error while reading file" << std::endl;
        return EXIT_FAILURE;
    }
    catch (const libconfig::ParseException& pex) {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return EXIT_FAILURE;
    }

    // initialize the 0MQ context
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
