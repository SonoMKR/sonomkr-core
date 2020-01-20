#ifndef SINEGENERATOR_H
#define SINEGENERATOR_H

#include <iostream>

#include <thread>
#include <string>
#include <math.h>
#include <unistd.h>
#include "ringbuffer.h"

class SineGenerator {
private:
    RingBuffer* _audioBuffer;
    std::thread _generatorThread;
    int _rate, _periodsize;
    bool _doSine;
    float _freq;
    double* _periodBuf;

public:
    SineGenerator(float freq, unsigned long size);
    ~SineGenerator();
    void run();
    void start();
    void stop();

    RingBuffer* getBuffer() {
        return _audioBuffer;
    }
};

#endif // SINEGENERATOR_H
