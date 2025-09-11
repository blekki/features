#include "stdlib.h"
#include "iostream"
#include <unistd.h>
#include "chrono"

#include "functional"
#include "memory"
#include "future"

#include "vector"
#include "queue"

using namespace std;

int getOne() {
    return 1;
}

void print() {
   cout << '!' << endl; 
}

void printPlus(char symbol) {
   cout << symbol << endl; 
}

float addf(float a, float b) {
    return a + b;
}

class TaskQueue
{
    private:
        queue<function<void()>> tasks;

    public:
        template<typename Func, typename... Args>
        auto addNewTask(Func&& func, Args&&... args) -> future<typename result_of<Func(Args...)>::type> {
            using return_type = typename result_of<Func(Args...)>::type;

            auto task = make_shared<packaged_task<return_type()>>(
                bind(func, args...)
            );
            future<return_type> result = task->get_future();

            auto newTask = [task]{
                (*task)();
            };
            
            tasks.push(newTask);
            return result;
        }

        void runNextTask() {
            if (!tasks.empty()) {
                (tasks.front())();
                tasks.pop();
            }
        }

};

int main() 
{
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    //==================================

    TaskQueue taskQueue;
    auto task1 = taskQueue.addNewTask(getOne); // return int
    auto task2 = taskQueue.addNewTask(print);
    auto task3 = taskQueue.addNewTask(printPlus, '#');
    auto task4 = taskQueue.addNewTask(addf, 4.1415f, 22.33f / 1.75f); // return float
    taskQueue.runNextTask();
    taskQueue.runNextTask();
    taskQueue.runNextTask();
    taskQueue.runNextTask();

    if (task1.valid())
        cout << task1.get() << endl;
    
    if (task4.valid())
        cout << task4.get() << endl;



    // using type = packaged_task<int()>;
    // shared_ptr<type> task = make_shared<type>(
    //     bind(getOne)
    // );
    // future<int> fut = task->get_future();
    // (*task)();
    // fut.wait();
    // cout << fut.get() << endl;


    //================================== check summary consume time
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
    cout << "\nTime was running (sec): " << time_span.count() << endl;
}