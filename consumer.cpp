#include "stdlib.h"
#include "iostream"
#include "chrono"
#include "math.h"
#include "vector"
#include "thread"

using namespace std;

double consumer(uint level) {
    if (level >= 5)
        return 0.0;

    // start consumming  >:)
    level++;
    
    double data;
    for (uint i = 0; i < 100; i++) {
        data = cos(time(0)) * sin(time(0)) +
               cos(time(0) + 3.1) * sin(time(0) + 3.1) +
               cos(time(0) + 2.1 * time(0)) * sin(time(0) + -4.7 * time(0));
    }

    for (uint i = 0; i < 8; i++)
        consumer(level);

    return data;
}

int main()
{   
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    
    vector<thread> threads;
    for (uint a = 0; a < 1000; a++) {
        threads.push_back(thread(consumer, 0));
    }

    for (auto& th : threads)
        th.join();

    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
    cout << time_span.count() << endl;
}