#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <thread>
#include <zmq.hpp>
#include <zmq_addon.hpp>

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

    zmq::context_t *zmq_context_;
    zmq::socket_t zmq_req_socket_;

    void initialize();
    void cleanup();

    void sendResponse(int status, const char *message);
    void sendResponseBody(int status, const char *message, const char *body);

  public:
    MainController(Configuration *config, zmq::context_t *zmq);
    ~MainController();
    void startChannels(bool restart = false);
    void startChannel(int channel, bool restart = false);
    void stopChannels();
    void stopChannel(int channel);

    void run();
    void start();
    void stop();
    void exit();

    void waitUntilDone();
};

#endif // MAINCONTROLLER_H
