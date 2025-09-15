#include "stdlib.h"
#include "iostream"
#include "chrono"
#include <unistd.h>
#include "vector"

#include "threadPool.h"

using namespace std;

int sum(int array[], uint array_size) {
    int sum = 0;
    for (int i = 0; i < array_size; i++) {
        sum += array[i];
    }
    return sum;
}

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
    
    // generate a vector with the numbers
    const uint NUMBERS_COUNT = 10;
    vector<int> numbers;
    for (uint i = 0; i < NUMBERS_COUNT; i++) {
        numbers.push_back(rand() % 10);
        cout << numbers.at(i) << " ";
    }
    cout << endl;

    // Separate the vector into the smaller parts
    // and sent it to the ThreadPool for the next operations
    // ps: (VECTOR_LENGTH % PART_LENGTH) must be equil zero (= 0)
    const uint ARRAY_SIZE = 2;
    vector<future<int>> pre_results;
    for (uint v = 0; v < NUMBERS_COUNT; v += ARRAY_SIZE) {
        // int array[ARRAY_SIZE];
        // for (uint a = 0; a < ARRAY_SIZE; a++) {
        //     array[a] = numbers[v + a];
        // }

        // sending
        // auto future_result = threadPool.addNewTask(sum, array, ARRAY_SIZE);
        // pre_results.push_back(move(future_result));

        vector<int> vec;
        for (uint a = 0; a < ARRAY_SIZE; a++) {
            vec.push_back(numbers[v + a]);
        }

        // sending
        auto future_result = threadPool.addNewTask(sumVec, vec);
        pre_results.push_back(move(future_result));

        // pre_results.back().wait();
        // this_thread::sleep_for(chrono::milliseconds(200));

        // ERROR: an itteration finished faster, 
        // then a command copy the future_result into the vector;

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