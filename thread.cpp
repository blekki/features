#include "stdlib.h"
#include "iostream"
#include "ctime"

#include "thread"

using namespace std;

void print(int value) {
    for (uint a = 0; a < 100; a++) {
        cout << value;
    }
    cout << endl;
}

// ########## MAIN PROGRAM ##########
int main()
{  
    time_t timeBegin, timeEnd;
    timeBegin = clock();

    // multithreading case
    thread th1(print, 4);
    thread th2(print, 3);
    th1.join();
    th2.join(); 
    
    // print time consumming results
    timeEnd = clock();
    float delta = float(timeEnd - timeBegin) / float(CLOCKS_PER_SEC);
    cout << "Time sec: " << delta << endl;
}