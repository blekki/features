#include "stdlib.h"
#include "iostream"
#include "chrono"
#include <unistd.h>
#include "math.h"

#include "vector"
#include "functional"
#include "future"

#include "thread"
#include "mutex"
#include "atomic"

#include "threadUnit.h"
#include "taskQueue.h"

using namespace std;

class ThreadPool
{
    private:
        vector<ThreadUnit> threads;
        TaskQueue taskQueue;

        unique_ptr<thread> task_separater;
        mutex queue_locker;
        
        atomic<bool> shutdown = false;

        const uint THREAD_COUNT = 4;
    
    private:
        void shutdownProcesses() {
            shutdown.store(true, memory_order_release);
            
            // join all threads
            // task_separater->shutdownProcess();
            // task_separater.shutdownProcess();
            if (task_separater->joinable())
                task_separater->join();

            for (auto& th : threads)
                th.shutdownProcess();
        }

    public:
        void addNewTask(function<int()> newTask) {
            queue_locker.lock();
            taskQueue.addNewTask(newTask);
            queue_locker.unlock();
        }

        void separateTasks() {
            while(shutdown.load(memory_order_acquire) == false) {
                for (auto& th : threads) {
                    // if task is busy 
                    if (th.isFree() == false)
                        continue;
                    
                    // if task is free
                    queue_locker.lock(); 
                    if (taskQueue.isNotEmpty()) {
                        th.changeTask(taskQueue.getFrontTask());
                        taskQueue.popFrontTask();
                    }
                    queue_locker.unlock();
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

        ThreadPool() {
            for (uint i = 0; i < THREAD_COUNT; i++)
                threads.push_back(ThreadUnit());
            
            // task_separater = thread(separateTasks);
            // auto task_for_separator = []{
            //     separateTasks();
            // };
            this->task_separater = make_unique<thread>(&ThreadPool::separateTasks);
            // task_separater = ThreadUnit(task_for_separator);
        };

        ~ThreadPool() {
            shutdownProcesses();
        }
};

int sum(int array[], uint array_size) {
    int sum = 0;
    for (int i = 0; i < array_size; i++) {
        sum += array[i];
    }
    return sum;
}

int main()
{   
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    //==================================
    
    // generate a vector with the numbers
    const uint VECTOR_LENGTH = 10000;
    vector<int> numbers;
    for (uint i = 0; i < VECTOR_LENGTH; i++) {
        numbers.push_back(rand() % 10);
    }

    // ThreadPool generation
    ThreadPool threadPool;

    // Separate the vector into the smaller parts
    // and sent it to the ThreadPool for the next operations
    // > VECTOR_LENGTH % PART_LENGTH must be equil zero (0)
    const uint PART_LENGTH = 10;
    for (uint v = 0; v < VECTOR_LENGTH / PART_LENGTH; v += PART_LENGTH) {
        // create part
        int part[PART_LENGTH];
        for (uint a = 0; a < PART_LENGTH; a++) {
            part[a] = numbers[v + a];
        }

        // sending
        // threadPool.addNewTask(task, futureResults);
    }
    
    //==================================
    // check summary consume time
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
    cout << "\n" << time_span.count() << endl;
}