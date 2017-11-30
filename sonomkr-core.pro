TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread -lasound -lconfig++

SOURCES += main.cpp \
    SignalProcessing/audiocapture.cpp \
    SignalProcessing/biquadfilter.cpp \
    SignalProcessing/iirfilter.cpp \
    Shared/ringbuffer.cpp \
    Shared/ringbufferconsumer.cpp \
    SignalProcessing/leq.cpp \
    SignalProcessing/audiobuffer.cpp

HEADERS += \
    SignalProcessing/audiocapture.h \
    SignalProcessing/biquadfilter.h \
    SignalProcessing/sos_coefficients.h \
    SignalProcessing/iirfilter.h \
    Shared/ringbuffer.h \
    Shared/ringbufferconsumer.h \
    SignalProcessing/leq.h \
    SignalProcessing/audiobuffer.h \
    defines.h \
    Dbus/Adaptors/controller.h \
    Dbus/Adaptors/channel.h

DISTFILES += \
    Dbus/sonomkr-dbus-interface.xml \
    Dbus/fr.sonomkr.controller.xml \
    Dbus/fr.sonomkr.channel.xml
