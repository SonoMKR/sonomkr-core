#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <thread>
#include <zmq.hpp>
#include <zmq_addon.hpp>

#include "configuration.h"
#include "spectrumchannel.h"
#include "audiobuffer.h"
#include "audiosource.h"

class MainController
{
  private:
    bool do_run_;
    std::thread run_thread_;

    Configuration *config_;
    AudioBuffer *audio_buffer_;
    AudioSource *audio_source_;
    bool source_start_deferred_ = false; // WAV: started with the first channel

    void startDeferredSource();

    bool is_ch1_active_;
    bool is_ch2_active_;
    SpectrumChannel *channel1_ = nullptr;
    SpectrumChannel *channel2_ = nullptr;

    zmq::context_t *zmq_context_;
    zmq::socket_t zmq_req_socket_;

    void initialize();
    void cleanup();

    void sendResponse(int status, std::string message);
    void sendResponseBody(int status, std::string message, std::string body);

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
