#include "maincontroller.h"

#include <iostream>
#include <boost/filesystem.hpp>

#include "audiobuffer.h"
#include "audiocapture.h"
#include "sinegenerator.h"
#include "spectrumchannel.h"
#include "configuration.h"

MainController::MainController(Configuration *config, zmqpp::context *zmq) :
    do_run_(false),
    config_(config),
    zmq_context_(zmq),
    zmq_req_socket_(*zmq, zmqpp::socket_type::reply)
{
    std::string endpoint = config_->controller_bind_;
    zmq_req_socket_.bind(endpoint.c_str());

    initialize();
}

MainController::~MainController()
{
    do_run_ = false;
    if (run_thread_.joinable()) {
        run_thread_.join();
    }
    zmq_req_socket_.close();

    cleanup();
}

void MainController::initialize()
{
    audio_buffer_ = new AudioBuffer(config_, zmq_context_);
    audio_capture_ = new AudioCapture(config_, audio_buffer_);
    audio_capture_->start();

    is_ch1_active_ = config_->getChannel1Config()->active;
    is_ch2_active_ = config_->getChannel2Config()->active;

    int channel_count = 0;
    if (is_ch1_active_)
    {
        channel1_ = new SpectrumChannel(config_,
                                        1,
                                        audio_buffer_->getChannelBuffer(channel_count),
                                        1024,
                                        zmq_context_);
        channel_count++;
    }
    if (is_ch2_active_)
    {
        channel2_ = new SpectrumChannel(config_,
                                        2,
                                        audio_buffer_->getChannelBuffer(channel_count),
                                        1024,
                                        zmq_context_);
        channel_count++;
    }
}

void MainController::cleanup()
{
    if (is_ch1_active_)
    {
        delete channel1_;
        channel1_ = nullptr;
    }
    if (is_ch2_active_)
    {
        delete channel2_;
        channel2_ = nullptr;
    }
    delete audio_capture_;
    audio_capture_ = nullptr;
    delete audio_buffer_;
    audio_buffer_ = nullptr;
}

void MainController::startChannels(bool restart)
{
    if (is_ch1_active_)
    {
        channel1_->start(restart);
    }
    if (is_ch2_active_)
    {
        channel2_->start(restart);
    }
}

void MainController::startChannel(int channel, bool restart)
{
    if (channel == 1 && is_ch1_active_)
    {
        channel1_->start(restart);
    }
    if (channel == 2 && is_ch2_active_)
    {
        channel2_->start(restart);
    }
}

void MainController::stopChannels()
{
    if (is_ch1_active_)
    {
        channel1_->stop();
    }
    if (is_ch2_active_)
    {
        channel2_->stop();
    }
}

void MainController::stopChannel(int channel)
{
    if (channel == 1 && is_ch1_active_)
    {
        channel1_->stop();
    }
    if (channel == 2 && is_ch2_active_)
    {
        channel2_->stop();
    }
}

void MainController::run()
{
    while (do_run_)
    {
        zmqpp::message msg;
        try {
            zmq_req_socket_.receive(msg);
        }
        catch (const std::exception& e) {
            std::cerr << "Zmq socket receive error : " << e.what() << std::endl;
        }
        if (!do_run_)
        {
            break;
        }
        int num_parts = msg.parts();
        if (num_parts < 1) {
            sendResponse(400, "Not enough message parts");
            continue;
        }
        std::string first_part = msg.get(0);
        if (first_part == "START" || first_part == "STOP")
        {
            if (num_parts < 2) {
                sendResponse(400, "Not enough message parts");
                continue;
            }
            std::string channel = msg.get(1);
            if (channel == "ALL") {
                if (first_part == "START") {
                    startChannels(true);
                }
                else if (first_part == "STOP") {
                    stopChannels();
                }
                sendResponse(200, "OK");
                continue;
            }
            else if (channel == "1" || channel == "2") {
                int channel_num = std::stoi( channel );
                if (first_part == "START") {
                    startChannel(channel_num, true);
                }
                else if (first_part == "STOP") {
                    stopChannel(channel_num);
                }
                sendResponse(200, "OK");
                continue;
            }
            sendResponse(400, "Bad Request");
            continue;
        }
        else if (first_part == "LOAD")
        {
            if (num_parts < 2) {
                sendResponse(400, "Not enough message parts");
                continue;
            }
            boost::filesystem::path path(msg.get(1));
            if (!boost::filesystem::is_regular_file(path) || !path.is_absolute()) {
                sendResponse(400, "File not found or is not an absolute path");
                continue;
            }
            stopChannels();
            cleanup();
            if (config_->loadConfig(path.string()) > 0) {
                sendResponse(400, "Config file not found or unable to parse");
            }
            else {
                sendResponse(200, "OK");
            }
            initialize();
            startChannels(true);
            continue;
        }
        else if (first_part == "GET")
        {
            if (num_parts < 3) {
                sendResponse(400, "Not enough message parts");
                continue;
            }
            std::string channel = msg.get(1);
            std::string setting = msg.get(2);
            ChannelConfig* channel_config;
            if (channel == "1")
            {
                channel_config = config_->getChannel1Config();
            }
            else if (channel == "2") {
                channel_config = config_->getChannel2Config();
            }
            else {
                sendResponse(404, "Channel number must be 1 or 2");
                continue;
            }
            if (setting == "ACTIVE") {
                sendResponseBody(200, "OK", std::to_string(channel_config->active).c_str());
                continue;
            }
            if (setting == "FMIN") {
                sendResponseBody(200, "OK", std::to_string(channel_config->fmin).c_str());
                continue;
            }
            if (setting == "FMAX") {
                sendResponseBody(200, "OK", std::to_string(channel_config->fmax).c_str());
                continue;
            }
            if (setting == "PERIOD") {
                sendResponseBody(200, "OK", std::to_string(channel_config->integration_period).c_str());
                continue;
            }
            if (setting == "SENSITIVITY") {
                sendResponseBody(200, "OK", std::to_string(channel_config->sensitivity).c_str());
                continue;
            }
            if (setting == "ZMQ") {
                sendResponseBody(200, "OK", channel_config->publish_bind.c_str());
                continue;
            }
            sendResponse(404, "Setting must be 'ACTIVE', 'FMIN', 'FMAX', 'PERIOD', 'ZMQ' or 'SENSITIVITY'");
            continue;
        }
        else if (first_part == "SET")
        {
            if (num_parts < 4) {
                sendResponse(400, "Not enough message parts");
                continue;
            }
            std::string channel = msg.get(1);
            std::string setting = msg.get(2);
            std::string value = msg.get(3);
            ChannelConfig* channel_config;
            if (channel == "1")
            {
                channel_config = config_->getChannel1Config();
            }
            else if (channel == "2") {
                channel_config = config_->getChannel2Config();
            }
            else {
                sendResponse(404, "Channel number must be 1 or 2");
                continue;
            }
            if (setting == "SENSITIVITY") {
                float sens = float(std::stod(value));
                channel_config->sensitivity = sens;
                if (channel == "1") {
                    channel1_->setSentivityCorrection(20 * log10f(sens * 0.05));
                }
                else if (channel == "2") {
                    channel2_->setSentivityCorrection(20 * log10f(sens * 0.05));
                }
                config_->saveCurrentConfig();
                sendResponseBody(200, "OK", std::to_string(channel_config->sensitivity).c_str());
                continue;
            }
            sendResponse(404, "Setting must be 'SENSITIVITY'");
            continue;
        }
        else {
            sendResponse(404, "Action must be 'START', 'STOP', 'LOAD', 'SET' or 'GET'");
            continue;
        }
        sendResponse(400, "Bad Request");
        continue;
    }
}

void MainController::sendResponse(int status, const char *message)
{
    zmqpp::message msg;
    msg.add(std::to_string(status));
    msg.add(message);
    try {
        zmq_req_socket_.send(msg);
    }
    catch (const std::exception& e) {
        std::cerr << "Zmq socket send error : " << e.what() << std::endl;
    }
}

void MainController::sendResponseBody(int status, const char *message, const char *body)
{
    zmqpp::message msg;
    msg.add(std::to_string(status));
    msg.add(message);
    msg.add(body);
    try {
        zmq_req_socket_.send(msg);
    }
    catch (const std::exception& e) {
        std::cerr << "Zmq socket send error : " << e.what() << std::endl;
    }
}

void MainController::start()
{
    do_run_ = true;
    bool autostart = config_->autostart_;
    if (autostart)
    {
        startChannels();
    }
    run_thread_ = std::thread(&MainController::run, this);
}

void MainController::stop()
{
    stopChannels();
    audio_capture_->stop();
    if (run_thread_.joinable()) {
        run_thread_.join();
    }
}

void MainController::exit()
{
    do_run_ = false;
    stop();
}

void MainController::waitUntilDone()
{
    if (run_thread_.joinable()) {
        run_thread_.join();
    }
}


