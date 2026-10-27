#include <iostream>
#include <thread>
#include <future>

void getValue(std::promise<int> promise) {
    promise.set_value(rand() & 10);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

int main() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future(); // connect future with promise

    std::thread th(getValue, std::move(promise));

    std::cout << future.get() << std::endl;

    th.join();
}