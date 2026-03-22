/* Voss-McCartney pink noise
 * Project Crew™ 1/27/2026
 *
 * Pros might wanna use low freq high pass.
 */

#include <iostream>
#include <cmath>
#include <random>
#include <array>
#include <cstdio>
using namespace std;

#define PROGNAME    "floyd"

/* >9 streams and we get the low frequency problem. */
#define N_STREAMS   9
#define LE          1
#define ENDIAN      LE                  // BE or LE
/* haven't tested changing it a lot */
#define RATE        44100
/* Clipping is no allowed. */
#define VOLMAX      32767 / N_STREAMS
/* I want to change bit the depth boy 16 is pretty good & more rewrite. */
/* maybe put out some wav header but ahh we probably need a struct */
/* so I just use some of the converter softwares 🤷 */


void
usage()
{
    cerr
        << endl
        << "  " PROGNAME" is a pink noise generator who uses the Voss-McCartney algorithm." << endl
        << "  default output is raw Signed, 16-bit, little endian, 44.1 kHz, Mono" << endl
        << "  Test the audio equipments, etc." << endl
        << endl
        << "  Usage:  $ " PROGNAME" [SECONDS] [redirect/pipe]" << endl
        << "  SECONDS is a whole number (integer) >= 1, sign ignored ( abs(SECONDS) )" << endl
        << "  for stereo, sampling is double, so use SECONDS *= 2" << endl
        << endl
        << "  Example:  $ " PROGNAME" 3 > sndfile.raw" << endl
        << "  Output should be 3 seconds of the pink noise" << endl
        << endl
        << "  If you run " PROGNAME" without args, he will keep going." << endl
        << "  You can use ^C to stop the " PROGNAME"." << endl
        << "  That mean you can do something like:  $ " PROGNAME" | aplay -f cd" << endl
        << "    and get the noise for a while." << endl
        << endl
        << "  (http://en.wikipedia.org/Pink_Noise)" << endl
        << "  Project Crew™ 2026" << endl
        << endl;
    exit(EXIT_FAILURE);
}

int
main(int argc, char* argv[])
{
    random_device r;                    // department 27
    static seed_seq seed { r(), r(), r(), r(), r(), r(), r(), r() };
    /* Don't change the pseudorandom engine unless you don't care about
     * whistling in the noise! They call "spectral characteristics"
     * in the doc. */
    mt19937 gen(seed);                  // we play some Twister
    static uniform_int_distribution<int16_t> d(-VOLMAX, VOLMAX);
    array<int16_t, N_STREAMS> streams;
    int16_t s[] {0};                    // sample
    int h, i, j, inc, dur, loopc;
    char ch = 0, cl = 0;
    ch += cl;                           // stop the unused var warning

    if(argc == 2)                       // arg processing...
    {
        dur = RATE / 256 * abs(atoi(argv[1]));
        if(!dur)
            usage();
        inc = 1;
    }else
    {
        dur = 1;                        // no arg = infine loop
        inc = 0;
    }

    loopc = pow(2, N_STREAMS - 1);      // set outer loop counter
    for(h = 0; h < dur; h += inc)       // optional infinite loop
    {
        for(i = 0; i < loopc; i++)      // here starts Voss-McCartney
        {
            for(j = 0; j < N_STREAMS; j++)
            {
                if(i % (1 << j) == 0)   // left shift 2 ^ j
                        streams[j] = d(gen);
            }
        /* let's fold up */
        s[0] = (int16_t)accumulate(streams.begin(), streams.end(), 0);
        /* output time   */
    #if ENDIAN == LE
        fwrite (s, 2, 1, stdout);
    #else
        cl = it[0] & 0xff;
        ch = it[0] >> 8;
        cout << ch << cl;
    #endif
        }
    }
/*
    cout << endl;
    for(uint16_t i : streams)
        cout << i << ", ";
    cout << endl;
*/
}