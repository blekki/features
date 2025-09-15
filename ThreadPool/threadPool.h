#pragma once
#include "stdlib.h"
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
        vector<unique_ptr<ThreadUnit>> threads; // our workers
        unique_ptr<thread> task_separater;      // sents tasks to the "workers"
        TaskQueue taskQueue;
        mutex queue_locker;

        atomic<bool> shutdown = false;
        
        const uint THREAD_COUNT = 100;
    
    private:
        void separateTasks() {
            while(shutdown.load(memory_order_acquire) == false) {
                for (auto& th : threads) {
                    // if task is busy
                    if (th->isFree() == false)
                        continue;
                    
                    // if task is free
                    queue_locker.lock();
                    if (taskQueue.isNotEmpty())
                        th->pushTask(taskQueue.getNextTask());
                    queue_locker.unlock();
                }
                this_thread::sleep_for(chrono::milliseconds(1));
            }
        }

    public:
        template<typename Func, typename... Args>
        auto addNewTask(Func func, Args... args) {
            // todo: push a task pack at a time moment (not the individual tasks)
            queue_locker.lock();
            auto future_result = taskQueue.pushTask(func, args...);
            queue_locker.unlock();
            return future_result;
        }

        void shutdownProcesses() { // safety way stop the processes of the current object
            shutdown.store(true, memory_order_release);

            // stop separator
            if (task_separater->joinable())
                task_separater->join();

            // stop other threads
            for (auto& th : threads)
                th->shutdownProcess();
        }

        ThreadPool() {
            for (uint i = 0; i < THREAD_COUNT; i++) {
                threads.push_back(make_unique<ThreadUnit>());
            }
            task_separater = make_unique<thread>(&ThreadPool::separateTasks, this);
        };

        ~ThreadPool() {
            shutdownProcesses();
        }
};