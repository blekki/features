#include "stdlib.h"
#include "iostream"

#define MAX(a, b) (a >= b ? a : b)
#define PRINT(str) {\
    cout << "here is your string: ";            \
    cout << str << endl;                        \
    cout << "string with a \'#\': ";            \
    cout << #str << endl; \
    }
#define GET(index) num_##index

using namespace std;

int main()
{
    // simple example
    int a = 10;
    int b = 7;
    cout << MAX(a, b) << endl;
    
    int c = MAX(a, b);
    cout << c << endl;

    // №2
    PRINT("my_string");

    // №3
    int num_1 = 10;
    int num_2 = 20;
    cout << "num_1 = " << GET(1) << endl;
    cout << "num_2 = " << GET(2) << endl;

}