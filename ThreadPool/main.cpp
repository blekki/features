#include "stdlib.h"
#include "iostream"
#include "chrono"
#include <unistd.h>
#include "vector"

#include "threadPool.h"

using namespace std;

int sumVec(vector<int> vec) {
    int sum = 0;
    for (int i = 0; i < vec.size(); i++) {
        sum += vec[i];
    }
    return sum;
}

int main() {
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    //==================================
    
    // preparation block
    ThreadPool threadPool;
    
    // properties
    const uint NUMBERS_COUNT = 10;
    const uint PART_SIZE = 2;

    // generate a vector with the numbers
    vector<int> numbers;
    for (uint i = 0; i < NUMBERS_COUNT; i++) {
        numbers.push_back(rand() % 10);
        cout << numbers.at(i) << " ";
    }
    cout << endl;

    // Separate the vector into the smaller "parts"
    // and sent it to the ThreadPool for the next operations
    // ps: (number_count % part.length()) must be equil zero (= 0)
    vector<future<int>> pre_results;
    for (uint v = 0; v < NUMBERS_COUNT; v += PART_SIZE) {
        vector<int> vec(PART_SIZE);
        for (uint a = 0; a < PART_SIZE; a++) {
            vec.push_back(numbers[v + a]);
        }

        // sending
        auto future_result = threadPool.addNewTask(sumVec, vec);
        pre_results.push_back(move(future_result));
    }

    for (uint a = 0; a < pre_results.size(); a++) {
        pre_results.at(a).wait();
        cout << pre_results.at(a).get() << " ";
    }
    

    //==================================
    // check summary consumed time
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
    cout << "\n" << time_span.count() << endl;
}