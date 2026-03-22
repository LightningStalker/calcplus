#include <iostream>
using namespace std;

int
main(int argc, char * argv[])
{
    int i, Number, max, count = 0;

    if (argc < 2)
    {
        cout << "I need a max number" << endl;
        cout << "like this  $ " << argv[0] << " 123" << endl;
        return 1;
    }

    max = atoi(argv[1]);

    for (Number = 2; Number <= max; Number++)
    {
        for (i = 2; i <= Number / 2; i++)
        {
            if (Number % i == 0)
            {
                count++;
                break;
            }
        }
        if (count == 0 && Number != 1)
        {
            cout << Number << ", ";
        }
        //else
        {
            //cout << Number << " is Not" << endl;
        }
        count = 0;
    }
    cout << "max reached" << endl;
    return 0;
}
