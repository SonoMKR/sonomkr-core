TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread -lasound -lconfig++ -lzmq -lzmqpp

QMAKE_CXXFLAGS += -O3

SOURCES += main.cpp \
    audiocapture.cpp \
    biquadfilter.cpp \
    iirfilter.cpp \
    ringbuffer.cpp \
    ringbufferconsumer.cpp \
    audiobuffer.cpp \
    sinegenerator.cpp \
    maincontroller.cpp \
    configuration.cpp \
    spectrumchannel.cpp \
    spectrum.cpp \
    leqfilter.cpp \
    antialiasingfilter.cpp

HEADERS += \
    audiocapture.h \
    biquadfilter.h \
    iirfilter.h \
    ringbuffer.h \
    ringbufferconsumer.h \
    audiobuffer.h \
    defines.h \
    sinegenerator.h \
    maincontroller.h \
    configuration.h \
    spectrumchannel.h \
    spectrum.h \
    leqfilter.h \
    antialiasingfilter.h

DISTFILES += \
    sonomkr.conf \
    filters.conf
