/* Capacitance + Voltage² -> joules calc
 * Project Crew™ 6/28/2025
 */

#include <iostream>
#include <cmath>
#include <exception>
using namespace std;

int
main(int argc, char * argv[])
{
    long double C, V;

    try
    {
        C = stod(argv[1]);
        V = stod(argv[2]);
    }catch (...)
    {
        cerr << "USAGE: $ " << argv[0] << " C(microfarad) V" << endl;
        return(EXIT_FAILURE);
    }

/* J = ½CV² */
    cout << 5e-7 * C * pow(V, 2.0) << endl;

    return(EXIT_SUCCESS);
}
