#include <iostream>

using namespace std;

int main()
{
    cout << "Hello World!" << endl;
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

            aliasing filter & decimation (48)[16Hz - 0.8Hz]
                x filters
                    iir filter
                        biquad filter

*/
