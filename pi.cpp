#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <quadmath.h>
#include <exception>
//#include <boost/multiprecision/cpp_bin_float.hpp>
using namespace std;

/*
 * I tried to learn math.
 * 3/2025 Project Crew™
 */

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

//Digits tallied after each OUTER loop
#define ITERATIONS_OUTER 50
#define ITERATIONS_INNER 5000  // 🡨 must be even number

//Total iterations = OUTER * INNER

//Nilakantha infinite series
class Nilakantha
{
    private:
    
    _Float128 sum = 0;
    _Float128 temp = 0;
    _Float128 a = 2, b = 3, c = 4;

    public:
    
    _Float128
    GetPI(int n)
    {
        int i;
        
        for (i = 0; i < n; i++)
        {
            temp = 4.0Q / (a * b * c);
            sum += i % 2 == 0 ? temp : -temp;
            a += 2; b += 2; c += 2;
        }
        return sum;
    }
};

// print _Float128
void
print(_Float128 fp)
{
    char buf[37 + sizeof(".e+99999")];
    int sz = strfromf128(buf, sizeof buf, "%.37g", fp);
    fwrite(buf, 1, sz, stdout);
}

// stringize()
string
stringeyes(_Float128 strug)
{
    char buf[37 + sizeof(".e+99999")];
    strfromf128(buf, sizeof buf, "%.37g", strug);
    return buf;
}

// compare float to macro as strings, return total == digits in fraction
int
strnlenpp(_Float128 PI)
{
    long unsigned int t;  // since .length() returns size_type
    string stringling;
    
    //how many digits found so far?
    stringling = stringeyes(PI);
    for(t = 2; t <= stringling.length(); t++)
    {
        if(stringling[t] != STRINGIZE(M_PIl)[t])
        {
            return (int) t - 2;
        }
    }
    cout << "All digits match" << endl;
    return 0;
}

int
main(int argc, char * argv[])
{
    typedef std::numeric_limits<_Float128> ldbl;

    _Float128 myPI = 0;
    unsigned int iterate, iter_outer;
    int PIlen = 0, lastval = 0;

    Nilakantha Nilkant;

    try
    {
        iter_outer = stoul(argv[1]);
    } catch (logic_error& e)
    {
        cout << "NO VALID ARG, USING DEFAULT LOOP COUNT" << endl;
        iter_outer = ITERATIONS_OUTER;
    }

    cout.precision(ldbl::max_digits10);
    cout << "🗸 Nilakantha infinite series" << endl;
    cout << "looking for π..." << endl;
    cout << "👁 ⏿ got       ";
    for(iterate = 1; iterate <= iter_outer; iterate++)
    {
        myPI = Nilkant.GetPI(ITERATIONS_INNER);
        PIlen = strnlenpp(myPI);
        if (PIlen > lastval)
        {
            cout << string(6, '\b') << PIlen << ".. digits" << flush;
            lastval = PIlen;
        }
    }

    cout << endl << string(PIlen, ' ') << " v" << endl;
    print(3 + myPI);
    cout << " 🡨 (this run)" << endl;
    //print(M_PIl);    
    //cout << endl;
    cout << STRINGIZE(M_PIl) << "\b 🡨 (precalc-ulated ref. value)" << endl; //<< "  ";
    //print(((_Float128) M_PIl - 3.0Q) * 1e36Q);
    //cout << endl;
    //cout << fixed << ' ' << truncf128(M_PIl * 1e36L) << endl;
}
    