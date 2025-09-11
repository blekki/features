#include "stdlib.h"
#include "iostream"
#include "ctime"

#include "thread"
#include "mutex"

using namespace std;

mutex mtx;

void print(int number) {
    unique_lock u_lock(mtx);
    u_lock.lock();
    for (uint i = 0; i < 10; i++)
        cout << number;
    cout << endl;
    u_lock.unlock();
    
}

// ########## MAIN PROGRAM ##########
int main()
{  
    time_t timeBegin, timeEnd;
    timeBegin = clock();

    // multithreading case
    thread th1(print, 1);
    thread th2(print, 2);
    th1.join();
    th2.join();

    // print time consumming results
    timeEnd = clock();
    float delta = float(timeEnd - timeBegin) / float(CLOCKS_PER_SEC);
    cout << "Time sec: " << delta << endl;
}