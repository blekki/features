#include "stdlib.h"
#include "iostream"
#include "ctime"

#include "thread"
#include "mutex"

using namespace std;

recursive_mutex mtx;

void lamda(int number, uint iter) {
    lock_guard un_lock(mtx);
    if (iter < 10) {
        cout << number;
        lamda(number, ++iter);
    }
}

void print(int number) {
    lamda(number, 0);
    cout << endl;
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