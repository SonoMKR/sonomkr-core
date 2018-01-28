#include "sinegenerator.h"


SineGenerator::SineGenerator(float freq, unsigned long size):
    _freq(freq)
{

    _audioBuffer = new RingBuffer<float>(size);

    _rate = 44100;

    _periodsize = 4410;
}

SineGenerator::~SineGenerator()
{
    _doSine = false;
    _generatorThread.join();
}

void SineGenerator::run()
{
    double lastTime = 0;
    _doSine = true;

    _periodBuf = new float[_periodsize];
    _audioBuffer->resetBuffer();

    int sleepTime = int((float(_periodsize) / _rate) * 1000 * 1000);

    while (_doSine) {

        for (int i = 0; i < _periodsize; i++) {
            _periodBuf[i] = sin(2.0 * 3.141592653589793 * _freq * lastTime);
            lastTime += 1.0 / float(_rate);
        }

        _audioBuffer->writeToBuffer(_periodBuf, _periodsize);

        usleep(sleepTime);
    }
    free(_periodBuf);
}

void SineGenerator::start()
{
    _doSine = true;
    _generatorThread = thread(&SineGenerator::run, this);
}

void SineGenerator::stop()
{
    _doSine = false;
}
