#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <thread>
#include <zmqpp/zmqpp.hpp>

#include "configuration.h"
#include "spectrumchannel.h"
#include "audiobuffer.h"
#include "audiocapture.h"

class MainController
{
  private:
    bool do_run_;
    std::thread run_thread_;

    Configuration *config_;
    AudioBuffer *audio_buffer_;
    AudioCapture *audio_capture_;

    bool is_ch1_active_;
    bool is_ch2_active_;
    SpectrumChannel *channel1_ = nullptr;
    SpectrumChannel *channel2_ = nullptr;

    zmqpp::context *zmq_context_;
    zmqpp::socket zmq_req_socket_;

    void initialize();
    void cleanup();

    void sendResponse(int status, const char *message);
    void sendResponseBody(int status, const char *message, const char *body);

  public:
    MainController(Configuration *config, zmqpp::context *zmq);
    ~MainController();
    void startChannels();
    void startChannel(int channel);
    void stopChannels();
    void stopChannel(int channel);

    void run();
    void start();
    void stop();
    void exit();

    void waitUntilDone();
};

#endif // MAINCONTROLLER_H
