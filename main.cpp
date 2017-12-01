#include <iostream>

#include "SignalProcessing/sos_coefficients.h"
#include "SignalProcessing/leq.h"
#include "SignalProcessing/audiobuffer.h"
#include "SignalProcessing/audiocapture.h"

using namespace std;

int main()
{
    AudioBuffer* buffer = new AudioBuffer(2, 48000 * 30);

    AudioCapture* catpure = new AudioCapture(buffer);

    Leq leq_1KHz(buffer->getChannelBuffer(0), 1024, NB_FILTERS, SOS_1kHz);

    catpure->start();
    leq_1KHz.start();

    cout << "Hello World!" << endl;

    while(true) {}
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

            [OPTION]
            aliasing filter & decimation (48)[16Hz - 0.8Hz]
                x filters
                    iir filter
                        biquad filter

*/
