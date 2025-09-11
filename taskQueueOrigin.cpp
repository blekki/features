#include "stdlib.h"
#include "iostream"
#include <unistd.h>
#include "chrono"

#include "vector"
#include "queue"

#include "future"
#include "functional"
#include "memory"

#include "thread"
#include "mutex"
#include "atomic"

using namespace std;

class TaskQueue
{
    private:
        queue<function<void()>> tasks;
        
    public:
        template<typename Func, typename... Args>
        auto addNewTask(Func func, Args... args) -> future<typename result_of<Func(Args...)>::type> {
            // Explanation:
            // Current method has "auto" as returning parameter because
            // we can't say right now what kind data type will be returned.
            // It's a dynamic data type.
            
            using return_type = typename result_of<Func(Args...)>::type;

            // our task
            auto task = make_shared<packaged_task<return_type()>>(
                bind(forward<Func>(func), forward<Args>(args)...)
            );

            // an instruction (function) which runs the task
            auto wrapped_task = [task]() { 
                (*task)(); 
            };

            future<return_type> result = task->get_future(); // future result
            tasks.push(wrapped_task); // save instruction

            return result; // return future<auto> result
        }

        void runNextTask() {
            if (!tasks.empty()) {
                (tasks.front())();
                tasks.pop();
            }
        }
};

void print() {
    cout << "A massage from the first function!" << endl;
}

int return5() {
    return 5;
}

int add(int a, int b) {
    return a + b;
}

int main()
{   
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    //========================

    TaskQueue taskQueue;
    auto result1 = taskQueue.addNewTask(print);
    auto result2 = taskQueue.addNewTask(return5);
    auto result3 = taskQueue.addNewTask(add, 4, 5);
    
    taskQueue.runNextTask();
    taskQueue.runNextTask();
    taskQueue.runNextTask();

    cout << result2.get() << endl;
    cout << result3.get() << endl;
    
    //========================
    // check summary consume time
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
    cout << "\n" << time_span.count() << endl;
}