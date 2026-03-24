/* the "Thiele/Small Box Alignment"
 * subwoofer box calculator
 * Project Crew™ 3/11/2026
 */

#define PROGNAME    "woofbox"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <libconfig.h++>
using namespace std;
using namespace libconfig;

/* I try{} using macro define constantes here, but some compiler bug
 *   multiply instead of divide.
 */
static const float
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
            os.seekp(ios_base::beg);                // reset
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

string
param(float f)
{
    ostringstream os;
    
    os << fixed << setprecision(3) << f;
    return os.str();
}  

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

void
usage()
{
    cerr << endl
         << "  " PROGNAME" is a woofer/subwoofer Thiele/Small Box Alignment"
            " calculator." << endl
         << "    I will tell you the ideal box size for the woofer." << endl
         << endl
         << "  use me like that:  $ " PROGNAME" [WOOFER CONFIG FILE]" << endl
         << "    default is `woofbox.cfg` in current directory" << endl
         << endl
         << "  see `woofbox.cfg.template`" << endl
         << endl;
    exit(EXIT_FAILURE);
}


int
main(int argc, char* argv[])
{
    float Vb, Fb, cd, w, h, d,
          Vas  = 1.0,
          Qts  = 1.0,
          Qtc  = 1.0,
          Fs   = 1.0,
          Sd   = 1.0,
          Xmax = 1.0;
    string name;
    Config cfg;
    fwf fwf;

    char file[256] = PROGNAME".cfg";
    if(argc == 2)
    {
        strncpy(file, argv[1], 256);
    }

// Read the config file. If there is an error, report it and exit.
    try
    {
        cfg.readFile(file);
    }catch(const FileIOException &fioex)
    {
        cerr << "I/O error while reading " << file << endl;
        usage();
    }catch(const ParseException &pex)
    {
        cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
        << " - " << pex.getError() << endl;
        return(EXIT_FAILURE);
    }

        const Setting& root = cfg.getRoot();

// Get thy Thiele/Small Parameters.
    try
    {
        name = (string)root.lookup("name");
        const Setting &param = root["Thiele-Small_Parameters"];
        Vas  = param.lookup("Vas");
        Qts  = param.lookup("Qts");
        Qtc  = param.lookup("Qtc");
        Fs   = param.lookup("Fs");
        Sd   = param.lookup("Sd");
        Xmax = param.lookup("Xmax");
    }catch(const SettingNotFoundException &nfex)
    {
        cerr << nfex.getPath() << " Setting is missing in " << root.getSourceFile() << endl;
    }

    cout << "   woofer: "  << name << endl
         << " Vas = " << param(Vas)
         << ", Qts = " << param(Qts)
         << ", Qtc = " << param(Qtc)
         << ", Fs = " << param(Fs)
         << ", Sd = " << param(Sd)
         << ", Xmax = " << param(Xmax) << endl
         << endl;
    
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