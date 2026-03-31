#define PROGNAME    "primes"

#include <iostream>
using namespace std;

int
main(int argc, char * argv[])
{
    int max, Number, i, count = 0;

    if(argc < 2)
    {
        cerr << endl
             << "  I need a max number" << endl
             << "  like this:  $ " << PROGNAME << " 123" << endl
             << endl;
        return EXIT_FAILURE;
    }

    max = atoi(argv[1]);

    if(max < 1)
        {
            cerr << "think positive" << endl;
            return EXIT_FAILURE;
        }

    cout << "1, ";
    for(Number = 2; Number <= max; Number++)
    {
        for(i = 2; i <= Number / 2; i++)
        {
            if(Number % i == 0)
            {
                count++;
                break;
            }
        }
        if(!count)
        {
            cout << Number << ", ";
        }/*else
        {
            cout << Number << " is Not" << endl;
        }*/
        count = 0;
    }
    cout << "max reached" << endl;
}