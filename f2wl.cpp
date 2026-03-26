/* frequency <-> wavelengths
 * Project Crew™ 3/18/2026
 */

#define PROGNAME    "f2wl"

#include <iostream>
#include <iomanip>
#include <cmath>
#include "lib/cxxopts/include/cxxopts.hpp"
using namespace std;

static inline constexpr double c    = 299792458;
static inline constexpr double c_Mm = c * 1e-6;

/* fill string with some chars */
string
fill(char c, int n)
{
    string s;
    s.resize(n, c);
    return s;
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

/* output functinos are here */
class out
{
    private:
        ostringstream os;
        int prec = 4, w = 11;

        void
        format()
        {
            os.seekp(ios_base::beg);    // reset
            os << setw(w);
        }

        string
        fwf(float f)                    // fixed width float
        {
            format();
            os << setprecision(prec) << f;
            return os.str();
        }
        
        string                          // inch format
        infs(float dft)
        {
            float in;
            short int fractz, nu, de;
            int ft, frac;

            ft      = floorf(dft);
            in      = (dft - ft) * 12;
            frac    = roundf((in - floorf(in)) * 32);
            if(frac == 0)
            {
                fractz = nu = de = 0;
            }else
            {
                fractz = ntz(frac);
                nu     = frac >> fractz;
                de     = 32   >> fractz;
            }
            format();
            os << setprecision(3)
               << setw(10) << dft << "ft    || "
               << setw(6)  << ft << "ft  + "
               << setw(2)  << (int)in << "   "
               << setw(2)  << nu << " / "
               << setw(2)  << de << "in";
            return os.str();
        }
        
    public:
        out()                           // initial
        {
            os << fixed;
        }

        void
        m(float result)                 // metric output
        {
            char p = 0;

            if(result < 1)
            {
                prec    = 2;
                p       = 'c';
                w       = 9;
                result *= 1e2;
            }
            cout << endl
                 << fwf(result) << p << "m @ 1\u03bb(MHz) ";
            if(p == 0)
                cout << ' ';
            cout << "  \u03bb/2 = " << fwf(result / 2) << p << "m   "
                    "  \u03bb/4 = " << fwf(result / 4) << p << "m" << endl
                 << fill(' ', 14 + w)
                 << "5/8\u03bb = " << fwf(result * 0.625) << p << "m    "
                    "60%\u03bb = " << fwf(result * 0.600) << p << "m" << endl
                 << fill(' ', 16 + w)
                 << "2\u03bb = " << fwf(result * 2) << p << "m      "
                    "4\u03bb = " << fwf(result * 4) << p << "m" << endl
                 << endl;
        }

        void
        in(float result)                // imperial units
        {
            float dft;
            dft     = result;
    
            cout << endl
                 << "    1\u03bb = " << infs(dft)  << endl
                 << "   \u03bb/2 = " << infs(dft / 2) << endl
                 << "   \u03bb/4 = " << infs(dft / 4) << endl
                 << "    2\u03bb = " << infs(dft * 2) << endl
                 << "    4\u03bb = " << infs(dft * 4) << endl
                 << "  5/8\u03bb = " << infs(dft * 0.625) << endl
                 << "  60%\u03bb = " << infs(dft * 0.600) << endl
                 << endl;
        }
};

/* how to use me */
void
usage(string help)
{
    cerr << help << endl
         << "  I can convert the frequency to a wavelength in free space." << endl
         << "    A wavelength in meters is convert back to frequency." << endl
         << endl;
    exit(EXIT_FAILURE);
}

/* boiler plate */
void
boil(float* freq)
{
    cout << fixed << setprecision(0)
     << endl
     << fill(' ', 10)
     << "For input frequency of " << *freq * 1e6 << " Hz";
}


int
main(int argc, char* argv[])
{
    float freq, freq_adj, result, vf;
    out out;
    cxxopts::ParseResult optres;
    cxxopts::Options options(PROGNAME,
        "\n  " PROGNAME" is frequency <-> wavelengths conversion\n");
    string help;

    options
        .positional_help("frequency")
        .allow_unrecognised_options()
        .add_options()
        ("h,help", "Print this usage we see here",
            cxxopts::value<bool>()->implicit_value("true",
            cxxopts::ImplicitArgPolicy::Disabled))
        ("i,inch", "Output the imperial format (feet & inches)\n"
                   " (Default is metric)",
            cxxopts::value<bool>()->implicit_value("true",
            cxxopts::ImplicitArgPolicy::Disabled))
/*      ("m,metric", "Output metric format",
            cxxopts::value<bool>()->implicit_value("true",
            cxxopts::ImplicitArgPolicy::Disabled))
*/      ("v,velocity-factor", "Scale result by velocity-factor %\n"
                              "For the antennas, use 95.0%. "
                              "Same as 468 / f.\n",
            cxxopts::value<float>()->default_value("100"), "X.XX")
        ("freq", "frequency in MHz", cxxopts::value<float>())
        ;
    options.parse_positional({"freq"});
    help = options.help();
    try
    {
        optres = options.parse(argc, argv);
    }catch(cxxopts::exceptions::specified_disabled_args& e)
    {
        cerr << PROGNAME": " << e.what() << endl;
        usage(help);
    }catch(cxxopts::exceptions::incorrect_argument_type& e)
    {
        cerr << PROGNAME": error: I need real numbers." << endl;
        usage(help);
    }catch(cxxopts::exceptions::missing_argument& e)
    {
        cerr << PROGNAME": error: " << e.what() << endl;
        usage(help);
    }

    if(optres.count("help"))
    {
        usage(help);
    }
    if(optres.count("freq"))
    {
        freq = optres["freq"].as<float>();
        if(freq < 9e-3)
        {
            cerr << PROGNAME": error: I need real numbers." << endl;
            usage(help);
        }
    }else
    {
        cerr << PROGNAME": error: I need a frequency in MHz." << endl;
        usage(help);
    }
    boil(&freq);                   // boiler plate
    vf = optres["velocity-factor"].as<float>();
    if(vf > 100)
    {
        cerr << PROGNAME": error: velocity factor > 100%." << endl;
        usage(help);
    }else if(vf < 1)
    {
        cerr << PROGNAME": error: velocity factor very small or negative." << endl;
        usage(help);
    }else if(vf < 100)
    {
        cout << "  @ " << vf << "% velocity:" << endl;
    }else
    {
        cout << ':' << endl;
    }

    freq_adj  = freq;
    freq_adj *= vf < 100 ? 100 / vf : 1;
    result = c_Mm / freq_adj;

    if(result > 0.26)
    {
        if(optres.count("inch"))
        {
            result /= 0.3048;
            out.in(result);
        }else  // metric
        {
            out.m(result);
        }
    }else
    {
        cout << endl
             << "  Short wavelengths detected.  "
                "Defaulting to metric conversions." << endl;
        out.m(result);
    }
}