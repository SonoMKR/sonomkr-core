#include "maincontroller.h"

#include "leqfilter.h"
#include "audiobuffer.h"
#include "audiocapture.h"
#include "sinegenerator.h"
#include "spectrumchannel.h"
#include "configuration.h"

MainController::MainController(Configuration* config, context* zmq):
    _doRun(false),
    _config(config),
    _zmqContext(zmq),
    _zmqReqSocket(*_zmqContext, socket_type::reply)
{
    string endPoint = _config->getSetting(string(CONTROLLERBIND_PATH)).c_str();
    _zmqReqSocket.bind(endPoint.c_str());

    _audioBuffer = new AudioBuffer(_config);
    _audioCapture = new AudioCapture(_config, _audioBuffer);
//    SineGenerator* sine = new SineGenerator(2000.0, 44100);
//    sine->start();

    _isCh1Active = _config->getSetting(string(CH1_ACTIVE_PATH));
    _isCh2Active = _config->getSetting(string(CH2_ACTIVE_PATH));

    int channelCount = 0;
    if (_isCh1Active) {
        _channel1 = new SpectrumChannel(_config, 1, _audioBuffer->getChannelBuffer(channelCount), 1024,
                                        _zmqContext);
        channelCount++;
    }
    if (_isCh2Active) {
        _channel2 = new SpectrumChannel(_config, 2, _audioBuffer->getChannelBuffer(channelCount), 1024,
                                        _zmqContext);
        channelCount++;
    }
}

MainController::~MainController()
{
    _doRun = false;
    _runThread.join();
    _zmqReqSocket.close();

    if (_isCh1Active) {
        delete _channel1;
    }
    if (_isCh2Active) {
        delete _channel2;
    }
    delete _audioCapture;
    delete _audioBuffer;
}

void MainController::startChannels()
{
    if (_isCh1Active) {
        _channel1->start();
    }
    if (_isCh2Active) {
        _channel2->start();
    }
}

void MainController::startChannel(int channel)
{
    if (channel == 1 && _isCh1Active) {
        _channel1->start();
    }
    if (channel == 2 && _isCh2Active) {
        _channel2->start();
    }
}

void MainController::stopChannels()
{
    if (_isCh1Active) {
        _channel1->stop();
    }
    if (_isCh2Active) {
        _channel2->stop();
    }
}

void MainController::stopChannel(int channel)
{
    if (channel == 1 && _isCh1Active) {
        _channel1->stop();
    }
    if (channel == 2 && _isCh2Active) {
        _channel2->stop();
    }
}

void MainController::run()
{
    while (_doRun) {
        message msg;
        _zmqReqSocket.receive(msg);
        if (!_doRun) {
            break;
        }
        cout << msg.get(0) << endl;
        if (msg.get(0) == "startAll") {
            startChannels();
            cout << "START ALL" << endl;
        }
        if (msg.get(0) == "stopAll") {
            stopChannels();
            cout << "STOP ALL" << endl;
        }
        _zmqReqSocket.send("OK");
    }
}

void MainController::start()
{
    _doRun = true;
    _audioCapture->start();
    bool autostart = _config->getSetting(AUTOSTART_PATH);
    if (autostart) {
        startChannels();
    }
    _runThread = thread(&MainController::run, this);
}

void MainController::stop()
{
    stopChannels();
    _audioCapture->stop();
}

void MainController::exit()
{
    _doRun = false;
    stop();
}

void MainController::waitUntilDone()
{
    _runThread.join();
}

