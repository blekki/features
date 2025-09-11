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
        thread th;
        atomic<bool> shutdown = false;

        function<void()> task;
        mutex task_locker;
        atomic<bool> task_free = true;
        // !!!!!!!!!!!!!!
        // don't know what todo with a function task atomaric :(
        
    public:
        void loop() {
            while(shutdown.load(memory_order_acquire) == false) {
                
                // copy task
                task_locker.lock();
                function<void()> currentTask = task;
                task_locker.unlock();

                if(currentTask != NULL) {
                    task_free.store(false, memory_order_release);
                    
                    currentTask(); // run task and after reset resources
                    currentTask = NULL;
                    task = NULL;
                    
                    task_free.store(true, memory_order_release);
                }
                this_thread::sleep_for(chrono::milliseconds(10));
                
                // void (*currentTask)() = task.load(memory_order_acquire);
                // auto currentTask = task.load(memory_order_acquire);
                // if(currentTask != nullptr) {
                //     currentTask();
                //     // reset resources;
                //     currentTask = nullptr;
                //     task.store(nullptr, memory_order_release);
                // }
                // this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

    public:
        bool isFree() {
            return task_free.load(memory_order_acquire);
        }
        
        void changeTask(function<void()> newTask) {
            task_locker.lock();
            this->task = newTask;
            task_locker.unlock();
        }
        
        void shutdownProcess() {
            shutdown.store(true, memory_order_release);
            if (th.joinable())
                th.join();
        }

        ThreadUnit(){
            th = thread(&ThreadUnit::loop);
        }

        ThreadUnit(function<void()> newTask){
            this->task = newTask;
            th = thread(&ThreadUnit::loop);
        }

        ~ThreadUnit(){
            shutdownProcess();
        }
};