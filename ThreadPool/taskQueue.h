#pragma once
#include "stdlib.h"

#include "queue"
#include "future"
#include "functional"
#include "memory"

using namespace std;

class TaskQueue
{
    private:
        queue<function<void()>> tasks;
        
    public:
        template<typename Func, typename... Args>
        auto pushTask(Func func, Args... args) -> future<typename result_of<Func(Args...)>::type> {
            // Explanation:
            // Current method has "auto" as returning parameter because
            // we can't say right now what kind data type will be returned.
            // It's a dynamic data type.
            
            using return_type = typename result_of<Func(Args...)>::type;

            // our task
            auto task = make_shared<packaged_task<return_type()>>(
                // bind(forward<Func>(func), forward<Args>(args)...)
                bind(func, args...)
            );

            // an instruction (function) which runs the task
            auto wrapped_task = [task]() { 
                (*task)();
            };

            future<return_type> result = task->get_future(); // future result
            tasks.push(wrapped_task); // save instruction

            return result; // return future<auto> result
        }

        bool isNotEmpty() {
            return !tasks.empty();
        }

        function<void()> getNextTask() {
            function<void()> task = tasks.front();
            tasks.pop();
            return task;
        }

        void popFrontTask() {
            tasks.pop();
        }

        TaskQueue(){}
};