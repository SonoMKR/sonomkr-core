TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    SignalProcessing/audiocapture.cpp \
    SignalProcessing/biquadfilter.cpp \
    SignalProcessing/iirfilter.cpp \
    Shared/ringbuffer.cpp \
    Shared/ringbufferconsumer.cpp

HEADERS += \
    SignalProcessing/audiocapture.h \
    SignalProcessing/biquadfilter.h \
    SignalProcessing/sos_coefficients.h \
    SignalProcessing/iirfilter.h \
    Shared/ringbuffer.h \
    Shared/ringbufferconsumer.h
