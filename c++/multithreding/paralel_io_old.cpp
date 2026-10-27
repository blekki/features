// Author comment:
// To run the app recommend use `clear && ./your_file.out`

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

std::atomic<bool> should_stop(false);
std::mutex mtx;

struct Cursor {
    int row = 0;
    int column = 0;

    Cursor() {};
    Cursor(int row, int column) : row(row), column(column) {};
    void place() {
        printf("\033[%d;%dH", row, column);
    }
};

void printer() {
    Cursor cursor(1, 10);
    const int MAX_ROW = 10;
    while (!should_stop.load(std::memory_order_relaxed)) {

        {
            std::lock_guard guard(mtx);
            cursor.place();
            std::cout << '-' << std::flush;
        }

        // replace log
        if (++cursor.row == MAX_ROW) {
            cursor.row = 1;     // reset row
            cursor.column++;    // add tiny offset
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    std::thread th(printer);
    
    std::string input = "";
    const Cursor cursorInput(0, 0);
    while (true) {
        char a;
        std::cin >> input;
        if (input == "exit") {
            should_stop.store(true, std::memory_order_relaxed);
            break;
        }
    }

    th.join();
}