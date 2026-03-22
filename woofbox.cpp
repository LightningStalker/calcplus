/* the "Thiele/Small Box Alignment"
 * subwoofer box calculator
 * Project Crew™ 3/11/2026
 */

#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

/* I try{} using macro define constantes here, but some compiler bug
 *   multiply instead of divide.
 */
static const float
    Vas  = 16.51,
    Qts  = 0.36,
    Fs   = 33,
    Qtc  = 0.707,
    Xmax = 9,
    Sd   = 148,               // surface displacement / cone area

    nP  = 2,                  // number ports
    Dp  = 5.0,                // port diameter (cm)

    c   = 34300,              // speed of sound @ 20°C (cm/s)
/* box dimensions ratio 1:1.2:1.5
 * polynomial eq: Vb = (1.5x * 1.2x * x) = (x³ * 1.5 * 1.2) = (1.8 * x³)
 * This is the truth as we know it.
 */
    R1 = 1.0,                 // R1 in ratios should be 1
    R2 = 1.2,
    R3 = 1.5,
    R  = R1 * R2 * R3;

/* fixed width float */
class fwf
{
    private:
        float bd_;                      // box dimension
        ostringstream os;

        void
        fomrat()
        {
            os.seekp(0);                // reset
            os << setw(6) << bd_;
        }

    public:
        fwf()                           // initial
        {
            os << fixed << setprecision(1);
        }

        string
        cm(float bd)
        {
            bd_ = bd * 10;
            fomrat();
            return os.str();
        }

        string
        inch(float bd)
        {
            bd_ = bd / 0.254;
            fomrat();
            return os.str();
        }
};

/* volume to height/width/depth dimension */
void
vol2dim(float* Vb, float* cd, float* w, float* h, float* d)
{
    *cd  = cbrtf(*Vb);                  // cubic dimension
    *cd *= 10;                          // turn into cm
    *w   = cbrtf(*Vb / R);              // solve for x
    *h   = *w * R2;
    *d   = *w * R3;
}


int
main(int argc, char* argv[])
{
    float Vb, Fb, cd, w, h, d;
    fwf fwf;

/* sealed */
    Vb = Vas / ( powf(Qtc / Qts, 2) - 1 );        // volume box
    Fb = Fs * (Qtc / Qts);                        // frequency ..
    vol2dim(&Vb, &cd, &w, &h, &d);

    cout << fixed << setprecision(1);
    cout << "   box type: Sealed" << endl
         << " volume: " << Vb << "L,  "
         << " cube size: " << cd << "cm,  "
         << " -3dB cutoff freq.: " << Fb << "Hz" << endl
         << endl;
    cout << " internal dimensions of 1:1.2:1.5 ratio:" << endl;
    cout << fwf.cm(w) << "W  x "
         << fwf.cm(h) << "H  x "
         << fwf.cm(d) << "D (cm)" << endl;

    cout << fwf.inch(w) << "W  x "
         << fwf.inch(h) << "H  x "
         << fwf.inch(d) << "D (in)" << endl;

/* vented */
    float Ap, Lv, Vd, Pv;

    Vb   = Vas * powf(Qts / 0.383, 3);
    Fb   = Fs * (0.383 / Qts);            // port tuning frequency
    Ap   = M_PI * powf(Dp / 2.0, 2) * nP; // total port area
    /* length vent */
    Lv   = powf(c / (2.0 * M_PI * Fb), 2) * Ap / (Vb * 1000) - 0.732 * Dp;
    Vd   = Sd * (Xmax / 10) * nP;         // volume displacement
    Pv   = Vd * 2.0 * M_PI * Fb / Ap / 100.0;     // max speed the air

    vol2dim(&Vb, &cd, &w, &h, &d);

    cout << "\n\n"
         << "   box type: Vented/Ported" << endl
         << " volume: " << Vb << "L,  "
         << " cube size: " << cd << "cm,  "
         << " -3dB cutoff freq.: " << Fb << "Hz" << endl
         << " number of ports: " << (int)nP << ",  "
         << " diameter of ports: " << Dp << "cm,  "
         << " length of ports: " << Lv << "cm" << endl
         << " ports total area: " << Ap << "cm³" << endl
         << " speaker displacement vol: " << Vd << "cm³,  "
         << " speed air in the port: " << Pv << "m/s" << endl
         << endl;
    cout << " internal dimensions of 1:1.2:1.5 ratio:" << endl;
    cout << fwf.cm(w) << "W  x "
         << fwf.cm(h) << "H  x "
         << fwf.cm(d) << "D (cm)" << endl;

    cout << fwf.inch(w) << "W  x "
         << fwf.inch(h) << "H  x "
         << fwf.inch(d) << "D (in)" << endl;
}