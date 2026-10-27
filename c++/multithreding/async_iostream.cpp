/*
    # Tip: Use [EXIT_CODE] to safety stop program.

    # How it works:
    # Create a new line and after move the last text in it.
    # A free place after movement is used for async std::cout.
    # You can continue use async std::cin without damaged text.
    # PS: That's bad practice. But if you want use async iostream without any libs it's your way.
    
    std::cin >> input;          // async std::cin
    std::cout                   // async std::cout
        << "\033[s\n\n"         // create a new line for a future moved text
        << "\033[2A\r\033[1L"   // move the prev line and use current for a new print
        << "Print async log"
        << "\033[u\033[1B"      // return to the last std::cin cursor position
        << std::flush;          // IMPORTANT print all changes
    std::cin >> input;          // continue use std::cin
*/

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

const char* EXIT_CODE = "exit";
std::atomic<bool> should_stop(false);

void printer() {
    uint i = 0;
    while (!should_stop.load(std::memory_order_acquire)) {
        std::cout
            << "\033[s\n\n"          // create new line for future moved text
            << "\033[2A\r\033[1L"    // move prev line and use current for new text
            << "auto print: " << i++ // text
            << "\033[u\033[1B"       // return to the end
            << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    std::thread th1(printer);
    
    std::string input;
    while (input != EXIT_CODE) {
        std::cout << "> ";
        std::getline(std::cin, input);
    }
    should_stop.store(true, std::memory_order_release);
    
    if (th1.joinable())
        th1.join();
}