#include <iostream>
#include <vector>
// multithreding
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Barrier
{
private:
    const uint maxThreads = 0;
    std::atomic<size_t> threadsCount = 0;
    
    std::mutex mtx;
    std::condition_variable conv;

public:
    Barrier(int maxThreads) : maxThreads(maxThreads) {};
    Barrier(const Barrier&) = delete;   // no copy constructor

    void wait() {
        if (threadsCount.fetch_add(1, std::memory_order_acq_rel) < (maxThreads - 1)) {  // one thread in waiting has to unlock other
            // lock thread
            std::unique_lock<std::mutex> ulock(mtx);
            conv.wait(ulock, [&]{ 
                return threadsCount.load(std::memory_order_acquire) >= maxThreads;
            });
        } else {
            // unlock all threads
            std::lock_guard<std::mutex> lockg(mtx);
            conv.notify_all();
        }
    }
};

void print(const char symbol) {
    std::cout << symbol;
}

void task(Barrier& barrier) {
    print('#');
    barrier.wait();
    print('@');
}

int main() {
    Barrier bar(10);

    // create threads
    size_t threadsCount = 10;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < threadsCount; i++) {
        threads.emplace_back(task, std::ref(bar));
    }

    // join threads
    for (size_t i = 0; i < threadsCount; i++) {
        if (threads[i].joinable())
            threads[i].join();
    }
    std::cout << "\n";
}