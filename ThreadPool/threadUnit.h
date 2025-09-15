#pragma once
#include "stdlib.h"

#include "functional"
#include "thread"
#include "atomic"

#include "mutex" // not sure


using namespace std;

class ThreadUnit
{
    private:
        // unique_ptr<thread> th;
        thread th = thread(&ThreadUnit::loop, this);
        function<void()> task = NULL;
        mutex task_locker;
        
        atomic<bool> task_free = true;
        atomic<bool> shutdown = false;
        
    private:
        void loop() {
            // while a command to stop didn't come
            while(shutdown.load(memory_order_acquire) == false) {
                function<void()> currentTask;

                // copy task
                task_locker.lock();
                currentTask = task;
                task = NULL;
                task_locker.unlock();

                if(currentTask != NULL) {
                    currentTask();      // run task
                    currentTask = NULL; // reset task
                    
                    task_free.store(true, memory_order_release); // make thread unbusy
                }

                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

    public:
        bool isFree() {
            return task_free.load(memory_order_acquire);
        }
        
        void pushTask(function<void()> newTask) {
            task_locker.lock();
            this->task = newTask;
            task_free.store(false, memory_order_release); // now task is busy
            task_locker.unlock();
        }
        
        void shutdownProcess() { // safety way stop the processes of the current object
            shutdown.store(true, memory_order_release);
            if (th.joinable())
                th.join();
        }

        // other constructs
        ThreadUnit(){}
        ThreadUnit(function<void()> newTask) {
            this->task = newTask;
        }

        // the main thread cannot be copied, so turn of possibilities try do it
        ThreadUnit& operator=(const ThreadUnit&) = delete;
        ThreadUnit& operator=(ThreadUnit&&) = default;
        ThreadUnit(const ThreadUnit&) = delete;
        ThreadUnit(ThreadUnit&&) = default;

        ~ThreadUnit() {
            shutdownProcess();
        }
};