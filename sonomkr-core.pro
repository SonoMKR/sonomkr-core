TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread -lasound -lconfig++ -ldbus-c++-1 -ldbus-1

SOURCES += main.cpp \
    SignalProcessing/audiocapture.cpp \
    SignalProcessing/biquadfilter.cpp \
    SignalProcessing/iirfilter.cpp \
    Shared/ringbuffer.cpp \
    Shared/ringbufferconsumer.cpp \
    SignalProcessing/audiobuffer.cpp \
    SignalProcessing/sinegenerator.cpp \
    maincontroller.cpp \
    configuration.cpp \
    spectrumchannel.cpp \
    spectrum.cpp \
    SignalProcessing/leqfilter.cpp \
    SignalProcessing/antialiasingfilter.cpp \
    SignalProcessing/sos_coefficients.cpp

HEADERS += \
    SignalProcessing/audiocapture.h \
    SignalProcessing/biquadfilter.h \
    SignalProcessing/sos_coefficients.h \
    SignalProcessing/iirfilter.h \
    Shared/ringbuffer.h \
    Shared/ringbufferconsumer.h \
    SignalProcessing/audiobuffer.h \
    defines.h \
    Dbus/Adaptors/controller.h \
    Dbus/Adaptors/channel.h \
    SignalProcessing/sinegenerator.h \
    maincontroller.h \
    configuration.h \
    spectrumchannel.h \
    spectrum.h \
    SignalProcessing/leqfilter.h \
    SignalProcessing/antialiasingfilter.h

DISTFILES += \
    Dbus/sonomkr-dbus-interface.xml \
    Dbus/fr.sonomkr.controller.xml \
    Dbus/fr.sonomkr.channel.xml
