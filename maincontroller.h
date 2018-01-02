#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <vector>
#include <thread>
#include <zmqpp/zmqpp.hpp>
#include <iostream>

class Configuration;
class SpectrumChannel;
class AudioBuffer;
class AudioCapture;

using namespace std;
using namespace zmqpp;

class MainController {
private:
    bool _doRun;
    thread _runThread;
    bool _isCh1Active;
    bool _isCh2Active;
    Configuration* _config;
    AudioBuffer* _audioBuffer;
    AudioCapture* _audioCapture;
    context* _zmqContext;
    socket _zmqReqSocket;
    SpectrumChannel* _channel1 = nullptr;
    SpectrumChannel* _channel2 = nullptr;

public:
    MainController(Configuration* config, context* zmq);
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
