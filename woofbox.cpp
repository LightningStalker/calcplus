/* the "Thiele/Small Box Alignment"
 * subwoofer box calculator
 * Project Crew™ 3/11/2026
 */

#define PROGNAME    "woofbox"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cstring>
#include <libconfig.h++>
using namespace std;
using namespace libconfig;

/* I try{} using macro define constantes here, but some compiler bug
 *   multiply instead of divide.
 */
static const float
    c   = 34300,              // speed of sound @ 20°C (cm/s)
/* box dimensions ratio 1:1.2:1.5
 * polynomial eq: Vb = (1.5x * 1.2x * x) = (x³ * 1.5 * 1.2) = (1.8 * x³)
 * This is the truth as we know it.
 */
    R1 = 1.0,                 // R1 in ratios should be 1
    R2 = 1.2,
    R3 = 1.5,
    R  = R1 * R2 * R3;

/* fill string with some chars */
string
fill(char c, int n)
{
    string s;
    s.resize(n, c);
    return s;
}

/* fixed width float */
class fwf
{
    private:
        float bd_;                      // box dimension
        ostringstream os, osi;

        void
        fomrat()
        {
            os.seekp(ios_base::beg);                // reset
            os << setw(6) << bd_;
        }

        /* number of trailing zeros (bits) */
        int
        ntz(unsigned short int x)
        {
            if (x == 0)
                return(32);

            int n = 1;

            if ((x & 0xF) == 0)
            {
                n = n  + 4;
                x = x >> 4;
            }
            if ((x & 0x3) == 0)
            {
                n = n  + 2;
                x = x >> 2;
            }
            return n - (x & 1);
        }


    public:
        fwf()                           // initial
        {
            os << fixed << setprecision(1);
        }

        string
        f3(float bd)
        {
            os.seekp(ios_base::beg);                // reset
            os << setprecision(3) << setw(6) << bd << setprecision(1);
            return os.str();
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

        string                          // inch format
        infs(float din)
        {
            float in;
            short int fractz, nu, de;
            int frac;

            din /= 0.254;
            in      = floorf(din);
            frac    = roundf((din - in) * 32);
            if(frac == 0)
            {
                fractz = nu = de = 0;
            }else
            {
                fractz = ntz(frac);
                nu     = frac >> fractz;
                de     = 32   >> fractz;
            }
            osi.seekp(ios_base::beg);
            osi << setw(4)  << (int)in << ' '
                << setw(2)  << nu << " / "
                << setw(2)  << de;
            return osi.str();
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

/* Bass Hit - Woofercooker */
int
main(int argc, char* argv[])
{
    float Vb, Fb, cd, w, h, d,
          Vas  = 1.0,
          Qts  = 1.0,
          Qtc  = 1.0,
          Fs   = 1.0,
          Sd   = 1.0,
          Xmax = 1.0,
          nP   = 1.0,
          dP   = 1.0,
          Wth  = 1.0;

    string name;
    Config cfg;
    fwf fwf;
    string cutfile = PROGNAME"cuts.txt";
    ofstream ostrm(cutfile, std::ios::binary);

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
        const Setting &tsparam = root["Thiele-Small_Parameters"];
        Vas  = tsparam.lookup("Vas");
        Qts  = tsparam.lookup("Qts");
        Qtc  = tsparam.lookup("Qtc");
        Fs   = tsparam.lookup("Fs");
        Sd   = tsparam.lookup("Sd");
        Xmax = tsparam.lookup("Xmax");
        const Setting &sysparam = root["System_Parameters"];
        nP   = floor((float)sysparam.lookup("nP"));
        dP   = sysparam.lookup("dP");
        Wth  = sysparam.lookup("Wth");
    }catch(const SettingNotFoundException &nfex)
    {
        cerr << nfex.getPath() << " Setting is missing in " << root.getSourceFile() << endl;
    }catch(const SettingTypeException &tex)
    {
        cerr << tex.getPath() << " Setting is wrong type in "
             << root.getSourceFile() << "\n  Did you use decimal point?" << endl;
        return(EXIT_FAILURE);
    }

    cout << "   Input config of woofer:  "  << name << endl
         << " Vas = "   << param(Vas) << "L"
         << ", Qts = "  << param(Qts)
         << ", Qtc = "  << param(Qtc)
         << ", Fs = "   << param(Fs) << endl
         << "  Sd = "   << param(Sd)
         << ", Xmax = " << param(Xmax)
         << ", nP = "   << nP
         << ", dP = "   << param(dP) << "cm"
         << ", Wth = "  << param(Wth) << "cm" << endl
         << endl;

    Wth /= 50.0;                                  // double thick in dm
/* sealed */
    Vb = Vas / ( powf(Qtc / Qts, 2) - 1 );        // volume box
    Fb = Fs * (Qtc / Qts);                        // frequency ..
    float Qtc_a = Qts * sqrtf(1 + Vas / Vb);
    vol2dim(&Vb, &cd, &w, &h, &d);

    cout << fixed << setprecision(1);
    cout << "   box type: Sealed";
    if(Qts >= 0.4 && Qts < 0.7)
        cout << "  0.4 < Qts < 0.7 this woofer is good for the sealed box";
    else if(Qts >= 0.7)
        cout << "  * LARGE BOX! *, Qts > 0.7, maybe mount free air.";
    cout << endl
         << " volume: " << Vb << "L,  "
         << " cube size: " << cd << "cm/side,  "
         << " system resonance freq.: " << Fb << "Hz" << endl
         << " system Q: " << fwf.f3(Qtc_a) << endl;

    ostrm << fixed << setprecision(1)
          << "Cut Sheet for " << name << ':' << endl
          << endl
          << "   box type: Sealed" << endl
          << " internal dimensions of 1:1.2:1.5 ratio:" << endl

          << fwf.cm(w) << "W  x "
          << fwf.cm(h) << "H  x "
          << fwf.cm(d) << "D (cm)" << endl

          << fwf.inch(w) << "W  x "
          << fwf.inch(h) << "H  x "
          << fwf.inch(d) << "D (in)" << endl
          << endl
          << "For " << Wth << "mm thick board:" << endl
          << "  Front and back sides: "
          << fwf.cm(w + Wth) << " x "
          << fwf.cm(h + Wth) << " cm" << endl
          << "    ..." << fill(' ', 17)
          << fwf.infs(w + Wth) << "\"  x "
          << fwf.infs(h + Wth) << "\" (in)" << endl
          << endl
          << "  Top and bottom:       "
          << fwf.cm(w + Wth) << " x "
          << fwf.cm(d) << " cm" << endl
          << "    ..." << fill(' ', 17)
          << fwf.infs(w + Wth) << "\"  x "
          << fwf.infs(d) << "\" (in)" << endl
          << endl
          << "  Left and right sides: "
          << fwf.cm(d) << " x "
          << fwf.cm(h) << " cm" << endl
          << "    ..." << fill(' ', 17)
          << fwf.infs(d) << "\"  x "
          << fwf.infs(h) << "\" (in)" << endl;

/* vented */
    float Ap, Lv, Vd, Pv;

    Vb   = Vas * powf(Qts / 0.383, 3);
    Fb   = Fs * (0.383 / Qts);            // port tuning frequency
    Ap   = M_PI * powf(dP / 2.0, 2) * nP; // total port area
    /* length vent */
    Lv   = powf(c / (2.0 * M_PI * Fb), 2) * Ap / (Vb * 1000) - 0.732 * dP;
    Vd   = Sd * (Xmax / 5.0);             // volume displacement
    Pv   = Vd * M_PI * Fb / Ap / 100.0;     // max speed the air

    vol2dim(&Vb, &cd, &w, &h, &d);

    cout << "\n\n"
         << "   box type: Vented/Ported";
    if(Qts < 0.4)
        cout << "  Qts < 0.4 this woofer is better for vented box";
    cout << endl
         << " volume: " << Vb << "L,  "
         << " cube size: " << cd << "cm/side,  "
         << " port tuning freq.: " << Fb << "Hz" << endl
         << " num. / ports: " << (int)nP << ",  "
         << " diameter of ports: " << fwf.f3(dP) << "cm,  "
         << " length each port: " << Lv << "cm" << endl
         << " ports total surface area: " << Ap << "cm\u00b2" << endl
         << " speaker displacement vol: " << Vd << "cm\u00b3,  "
         << " highest port air speed: " << Pv << "m/s" << endl;
    if(Pv > 17.0)
        cout << " * THE PORT WILL BE NOISY! * Use bigger ports/more ports." << endl;
    cout << endl;

    ostrm << "\n\n"
          << "   Choose Sealed or Ported and delete one before you print" << endl
          << fill('-', 50) << endl
          << "   box type: Vented/Ported"
          << endl
          << " internal dimensions of 1:1.2:1.5 ratio:" << endl

          << fwf.cm(w) << "W  x "
          << fwf.cm(h) << "H  x "
          << fwf.cm(d) << "D (cm)" << endl

          << fwf.inch(w) << "W  x "
          << fwf.inch(h) << "H  x "
          << fwf.inch(d) << "D (in)" << endl
          << endl
          << "For " << Wth << "mm thick board:" << endl
          << "  Front and back sides: "
          << fwf.cm(w + Wth) << " x "
          << fwf.cm(h + Wth) << " cm" << endl
          << "    ..." << fill(' ', 17)
          << fwf.infs(w + Wth) << "\"  x "
          << fwf.infs(h + Wth) << "\" (in)" << endl
          << endl
          << "  Top and bottom:       "
          << fwf.cm(w + Wth) << " x "
          << fwf.cm(d) << " cm" << endl
          << "    ..." << fill(' ', 17)
          << fwf.infs(w + Wth) << "\"  x "
          << fwf.infs(d) << "\" (in)" << endl
          << endl
          << "  Left and right sides: "
          << fwf.cm(d) << " x "
          << fwf.cm(h) << " cm" << endl
          << "    ..." << fill(' ', 17)
          << fwf.infs(d) << "\"  x "
          << fwf.infs(h) << "\" (in)" << endl;
    cout  << "   Sent cut sheet to file: " << cutfile << endl;
}