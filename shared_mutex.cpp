#include "stdlib.h"
#include "iostream"
#include "ctime"
#include "vector"

#include "thread"
#include "shared_mutex"

using namespace std;

shared_mutex sh_mtx;

void printUpper(char symbol) {
    lock_guard<shared_mutex> mt(sh_mtx);
    for (uint a = 0; a < 2; a++) {
        cout << symbol;
    }
}

void printLower(char symbol) {
    shared_lock<shared_mutex> mt(sh_mtx);
    for (uint a = 0; a < 5; a++) {
        cout << symbol;
        this_thread::sleep_for(100ms);
    }
}

// ########## MAIN PROGRAM ##########
int main()
{  
    time_t timeBegin, timeEnd;
    timeBegin = clock();

    // multithreading case
    vector<thread> vec;
    for (uint a = 0; a < 5; a++)
        vec.push_back(thread(printLower, 'a'));
    
    vec.push_back(thread(printUpper, 'B'));
    vec.push_back(thread(printUpper, 'C'));

    for (uint a = 0; a < 5; a++)
        vec.push_back(thread(printLower, 'd'));
    
    for (auto& th : vec)
        th.join();

    cout << endl;
    // print time consumming results
    timeEnd = clock();
    float delta = float(timeEnd - timeBegin) / float(CLOCKS_PER_SEC);
    cout << "Time sec: " << delta << endl;
}