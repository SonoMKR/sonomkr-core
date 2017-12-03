#include <iostream>

#include "SignalProcessing/leqfilter.h"
#include "SignalProcessing/audiobuffer.h"
#include "SignalProcessing/audiocapture.h"
#include "SignalProcessing/sinegenerator.h"

#include "spectrumchannel.h"

using namespace std;

int main()
{
    AudioBuffer* buffer = new AudioBuffer(1, 48000 * 30);

    AudioCapture* catpure = new AudioCapture(buffer);
    catpure->start();
//    SineGenerator* sine = new SineGenerator(2000.0, 44100);
//    sine->start();

    SpectrumChannel* channelOne = new SpectrumChannel(1, 0, 0, buffer->getChannelBuffer(0), 4096);
//    SpectrumChannel* channelOne = new SpectrumChannel(1, 0, 0, sine->getBuffer(), 4096);

    channelOne->start();

    cout << "Hello World!" << endl;

    channelOne->waitUntilDone();
    return 0;
}

/*

input samples (48000)[20k - 2kHz]
    x filters
        iir filter
            biquad filter

    aliasing filter & decimation (4800)[1.6kHz - 200Hz]
        x filters
            iir filter
                biquad filter

        aliasing filter & decimation (480)[160Hz - 20Hz]
            x filters
                iir filter
                    biquad filter

            [OPTION] ONLY POSSIBLE IF (fs / 1000.0) IS AN INT
            aliasing filter & decimation (48)[16Hz - 0.8Hz]
                x filters
                    iir filter
                        biquad filter

*/
