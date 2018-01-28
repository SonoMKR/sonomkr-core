bin_PROGRAMS=sonomkrd

sonomkrd_SOURCES= \
	antialiasingfilter.cpp \
	antialiasingfilter.h \
	audiobuffer.cpp \
	audiobuffer.h \
	audiocapture.cpp \
	audiocapture.h \
	biquadfilter.cpp \
	biquadfilter.h \
	configuration.cpp \
	configuration.h \
	defines.h \
	iirfilter.cpp \
	iirfilter.h \
	leqfilter.cpp \
	leqfilter.h \
	maincontroller.cpp \
	maincontroller.h \
	main.cpp \
	Makfile.am \
	ringbufferconsumer.cpp  \
	ringbufferconsumer.h  \
	ringbuffer.cpp  \
	ringbuffer.h  \
	sinegenerator.cpp  \
	sinegenerator.h \
	spectrumchannel.cpp \
	spectrumchannel.h \
	spectrum.cpp \
	spectrum.h

AM_LDFLAG = -lasound -lconfig++ -L/usr/local/lib -lzmqpp -lzmq -pthread

AM_CXXFLAGS = -std=c++1y
