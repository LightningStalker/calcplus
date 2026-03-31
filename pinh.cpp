/* Pinhole Camera lens
 * Project Crew™ 3/31/2026
 */

#define PROGNAME "pinh"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <exception>
using namespace std;

void
usage()
{
    cerr << endl
         << "  " PROGNAME" is a pinhole camera aperture calculator" << endl
         << "  Output is pinhole diameter in millimeters" << endl
         << "  Option -l finds focal length of the pinholes" << endl
         << endl
         << "    Usage:  $ " PROGNAME" wavelength(nm) focal_length(m)" << endl
         << "       or:  $ " PROGNAME" -l wavelength(nm) diameter(mm)" << endl
         << endl
         << "  550nm is good rule of thumb for the visible spectrum." << endl
         << "  Example:  $ " PROGNAME" 413.000 0.95\n" << endl
         << "  Output should be: 1.253mm\n" << endl
         << endl
         << "  If you discover a hole, -l will give its focal length in meters" << endl
         << "       so:  $ " PROGNAME" -l 568.000 3.3705" << endl
         << "  Output should be: 5.000m" << endl
         << endl;
    exit(EXIT_FAILURE);
}


int
main(int argc, char * argv[])
{
    string arg;

    cout << fixed << setprecision(3);

    if(argc == 3)
    {
        try
        {
            cout << 2e3 * sqrtf( stof(argv[2]) * ( stof(argv[1]) /
                                                   1e9 )
                               ) << "mm" << endl;
        }catch (...)
        {
            cerr << PROGNAME": ERROR: processing argument" << endl;
            usage();
        }
    }else if(argc == 4 && (arg = argv[1]) == "-l")
    {
        try
        {
            cout << pow( stof(argv[3]) /
                         2e3, 2 ) /

                    ( atof(argv[2]) /
                      1e9 ) << 'm' << endl;
        }catch (...)
        {
            cerr << PROGNAME": ERROR: processing argument" << endl;
            usage();
        }
    }else
    {
        usage();
    }
}