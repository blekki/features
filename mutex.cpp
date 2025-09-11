#include "stdlib.h"
#include "iostream"
#include "ctime"

#include "thread"
#include "mutex"

using namespace std;

mutex mtx;
string str = "";

void print(char symbol) {
    mtx.lock();
    str += symbol;
    mtx.unlock();

    // if (mtx.try_lock())
    //     str += symbol;
    // mtx.unlock();
}

// ########## MAIN PROGRAM ##########
int main()
{  
    time_t timeBegin, timeEnd;
    timeBegin = clock();

    // multithreading case
    thread th1(print, 'A');
    thread th2(print, 'B');
    thread th3(print, 'C');
    thread th4(print, 'D');
    th1.join();
    th2.join();
    th3.join();
    th4.join();

    cout << str << endl;
    
    // print time consumming results
    timeEnd = clock();
    float delta = float(timeEnd - timeBegin) / float(CLOCKS_PER_SEC);
    cout << "Time sec: " << delta << endl;
}