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
    const uint NUMBERS_COUNT = 10000;
    const uint PART_SIZE = 20;

    // generate a vector with the numbers
    int quick_sum = 0;
    vector<int> numbers;
    cout << "All numbers:" << endl;
    for (uint i = 0; i < NUMBERS_COUNT; i++) {
        int num = rand() % 10;
        numbers.push_back(num);
        cout << numbers.at(i) << " ";

        quick_sum += num;
    }
    cout << endl;

    // Separate the vector into the smaller "parts"
    // and sent it to the ThreadPool for the next operations
    // ps: (number_count % part.length()) must be equil zero (= 0)
    vector<future<int>> fResults;
    for (uint v = 0; v < NUMBERS_COUNT; v += PART_SIZE) {
        vector<int> vec(PART_SIZE);
        for (uint a = 0; a < PART_SIZE; a++) {
            vec.push_back(numbers[v + a]);
        }

        // sending
        auto future_result = threadPool.addNewTask(sumVec, vec);
        fResults.push_back(move(future_result));
    }

    // ready get final results
    vector<int> pre_results;
    cout << "\nPre-results:" << endl;
    for (uint a = 0; a < fResults.size(); a++) {
        fResults[a].wait();
        pre_results.push_back(fResults[a].get());
        cout << pre_results[a] << " ";
    }
    cout << endl;

    cout << "\nFinal-result (must be " << quick_sum << "):" << endl;
    int final_results;
    {
        auto future_result = threadPool.addNewTask(sumVec, pre_results);
        future_result.wait();
        final_results = future_result.get();
        cout << final_results;
    }
    cout << endl;
    

    //==================================
    // check summary consumed time
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
    cout << "\n" << time_span.count() << endl;
}