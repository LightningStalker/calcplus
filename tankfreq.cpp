/* compile with $ g++ -Wall -Os -o tankfreq tankfreq.cpp
 */

#include <iostream>
#include <iomanip>
#include <cmath>
#include <exception>
#include <locale> // std::locale, std::numpunct, std::use_facet
using namespace std;

// custom numpunct with grouping:
struct my_numpunct : numpunct<char> {
    string
    do_grouping() const {
        return "\03";
    }
};

class TankFreq {
private:
double L,
       C;

public:
void
args(double H, double F)
{
    L = H;
    C = F;
}

double
calc()
{
    return
        1 /
        (
        M_PI * 2 * sqrt(
            L * C * 1e-12
            )
        );
}

void
usage()
{
    cerr
        << "tankfreq is an LC tank resonance frequency calculator." << endl
        << "output is frequency in Hz" << endl
        << endl
        << "Usage: tankfreq microfarads microhenries" << endl
        << "Example: tankfreq 3.3 0.86207" << endl
        << "Output should be: 94,360.861" << endl;
}

};

int
main(int argc, char * argv[])
{
    TankFreq tf;

    if (argc == 3)
    {
        // set up digit grouping
        locale loc(cout.getloc(), new my_numpunct);
        cout.imbue(loc);

        // initialize vars
        tf.args(0, 0);

        try
        {
            tf.args(stod(argv[1]), stod(argv[2]));
        } catch (invalid_argument& e)
        {
            cerr << "error: arguments must be numeric." << endl;
            return(EXIT_FAILURE);
        }

        // 3 decimal places
        cout << fixed << setprecision(3) << tf.calc() << endl;

    }else
    {
        tf.usage();
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}
