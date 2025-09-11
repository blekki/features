#include "stdlib.h"
#include "iostream"
#include "ctime"
#include "vector"

#include "thread"
#include "atomic"

using namespace std;

uint badCount {0};
atomic<uint> atomicCount {0};

void badCounter() {
    for (uint a = 0; a < 1000; a++)
        badCount += 1;
}

void atomicCounter() {
    for (uint a = 0; a < 1000; a++)
        atomicCount += 1;
}

// ########## MAIN PROGRAM ##########
int main()
{  
    time_t timeBegin, timeEnd;
    timeBegin = clock();

    
    // create threads
    vector<thread> vec;
    for (uint a = 0; a < 160; a++)
        vec.push_back(thread(badCounter));
    
    for (uint a = 0; a < 160; a++)
        vec.push_back(thread(atomicCounter));
    
    // wait until all threads finished work
    for (auto& th : vec)
        th.join();
    
    // print results
    cout << "badCount = " << badCount << endl;
    cout << "atomicCount = " << atomicCount << endl;

    
    // print time consumming results
    cout << endl;
    timeEnd = clock();
    float delta = float(timeEnd - timeBegin) / float(CLOCKS_PER_SEC);
    cout << "Time sec: " << delta << endl;
}