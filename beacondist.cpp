/* distance to station based on RSSI and reference station
 * stations must be very very close to same
 * Project Crew™ 9/3/2025
 */

#include <iostream>
#include <cmath>
#include <exception>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

int
main(int argc, char * argv[])
{
    float ref,
          d,
          N,
          measured;

    string progname;


    try
    {
        fs::path argv0(argv[0]);
        /* strip dirname and extension */
        progname = (argv0.stem().string());

        if (argc > 5)
            /* too many args, throw excretion */
            throw invalid_argument("too many args");

        /* get the args */
        ref      = stof(argv[1]);
        d        = stof(argv[2]);
        N        = stof(argv[3]);
        measured = stof(argv[4]);

        /* output */
        cout << fixed << setprecision(1)
        /* we are servants of our formulaic ways */
             << powf(10.0, ((ref - measured) /
                           (10.0 * N))) * d << endl;
    }catch (...)
    {
        /* usage */
        cerr << endl
             << "  " << progname << " can find you about what distance to the transmitter"  << endl
             << "    base on the RSSI.  Reference value from a station close to same" << endl
             << "    build is use to compare the wave." << endl
             << endl
             << "  Path-loss exponent (N):      Accounts for signal loss due to environmental" << endl
             << "                                 factors, ranging from 2 in open spaces to 4 in" << endl
             << "                                 more obstructed environments." << endl
             << endl
             << "  USAGE: $ " << progname << " [RefRSSI] [RefDistance] [Path-loss exponent (N)] [RSSI]" << endl
             << endl;
        return(EXIT_FAILURE);
    }


    return(EXIT_SUCCESS);
}
