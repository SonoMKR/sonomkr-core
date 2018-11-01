#include "maincontroller.h"

#include <iostream>

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
    std::string endpoint = config_->getSetting(std::string(CONTROLLERBIND_PATH)).c_str();
    zmq_req_socket_.bind(endpoint.c_str());

    audio_buffer_ = new AudioBuffer(config_, zmq);
    audio_capture_ = new AudioCapture(config_, audio_buffer_);
    //    SineGenerator* sine = new SineGenerator(2000.0, 44100);
    //    sine->start();

    is_ch1_active_ = config_->getSetting(std::string(CH1_ACTIVE_PATH));
    is_ch2_active_ = config_->getSetting(std::string(CH2_ACTIVE_PATH));

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

MainController::~MainController()
{
    do_run_ = false;
    run_thread_.join();
    zmq_req_socket_.close();

    if (is_ch1_active_)
    {
        delete channel1_;
    }
    if (is_ch2_active_)
    {
        delete channel2_;
    }
    delete audio_capture_;
    delete audio_buffer_;
}

void MainController::startChannels()
{
    if (is_ch1_active_)
    {
        channel1_->start();
    }
    if (is_ch2_active_)
    {
        channel2_->start();
    }
}

void MainController::startChannel(int channel)
{
    if (channel == 1 && is_ch1_active_)
    {
        channel1_->start();
    }
    if (channel == 2 && is_ch2_active_)
    {
        channel2_->start();
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
        zmq_req_socket_.receive(msg);
        if (!do_run_)
        {
            break;
        }
        std::cout << msg.get(0) << std::endl;
        if (msg.get(0) == "startAll")
        {
            startChannels();
            std::cout << "START ALL" << std::endl;
        }
        if (msg.get(0) == "stopAll")
        {
            stopChannels();
            std::cout << "STOP ALL" << std::endl;
        }
        zmq_req_socket_.send("OK");
    }
}

void MainController::start()
{
    do_run_ = true;
    audio_capture_->start();
    bool autostart = config_->getSetting(AUTOSTART_PATH);
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
}

void MainController::exit()
{
    do_run_ = false;
    stop();
}

void MainController::waitUntilDone()
{
    run_thread_.join();
}
