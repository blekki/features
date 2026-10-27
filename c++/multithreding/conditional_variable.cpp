// ##############
// Comment: Not the best example of using conditional variable 
// ##############

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <list>

std::list<int> list;
std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> should_stop(false);

void consume() {
    while (!should_stop.load() && !list.empty()) {
        std::unique_lock lock(mtx);
        cv.wait(lock, []{
            return !list.empty();
        });
        
        for (auto &num : list)
            std::cout << num << " ";
        std::cout << std::endl;
        list.clear();
    }
}

void produce() {
    while (!should_stop.load()) {
        // insert data
        {
            std::lock_guard<std::mutex> lck(mtx);
            list.push_back(rand() % 10);
            std::cout << "size: " << list.size() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        cv.notify_one();
    }
}

int main() {
    std::thread th_consome(consume);
    std::thread th_produce(produce);


    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // spent time as complex algorithm
    should_stop.store(true);

    if (th_consome.joinable())
        th_consome.join();
    if (th_produce.joinable())
        th_produce.join();
}