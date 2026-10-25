// Pseudo-code about the client and server sides of network.
// Compile: [g++ -std=c++20 client_server.cpp -o client_server]
// Run server side: [./client_server -s]
// Run client side: [./client_server -c]

#include <stdlib.h>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
// thread
#include <atomic>
#include <thread>
// networking
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

// #define ADDITIONAL_POLLMSG

std::atomic<bool> should_stop(false);
const char* EXIT_CODE = "exit";

static const sockaddr_in server_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(8080),            // random port (it looks nice)
    .sin_addr = htonl(INADDR_LOOPBACK), // local host (127.0.0.1)
    .sin_zero = {}
};

class SockGuard
{
private:
    int sock = 0;

public:
    void initAsReciever() {
        resetSock();  // close and clear prev socket

        // create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock <= 0) {
            throw std::runtime_error("[err]: can't create socket");
        }
        
        // bind socket to own ip/port
        sockaddr_in addr = server_addr;
        if (bind(sock, (sockaddr*) &addr, sizeof(addr)) < 0) {
            resetSock();
            throw std::runtime_error("[err]: can't bind socket");
        }

        if (listen(sock, 10) < 0) {
            resetSock();
            throw std::runtime_error("[err]: can't start listen");
        }
    }

    void initAsSender() {
        resetSock();  // close and clear prev socket

        // create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock <= 0) {
            throw std::runtime_error("[err]: can't create socket");
        }

        // connect socket to the server addr
        sockaddr_in addr = server_addr;
        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
            resetSock();
            throw std::runtime_error("[err]: can't connect to server");
        }
    }

    int getSock() {
        return sock;
    }
    
    void resetSock() {
        if (sock) { // close prev socket
            close(sock);
        }
        sock = 0;   // reset
    }

    ~SockGuard() {
        resetSock();
    }
};

class Server
{
private:
    SockGuard server_sockguard;
    std::vector<pollfd> pollfds {};

    static const size_t BUFFER_SIZE = 128;
    char buffer[BUFFER_SIZE];

public:
    void resive() {
        // push server socket into poll
        pollfd server;
        server.fd = server_sockguard.getSock();
        server.events = POLLIN;
        pollfds.push_back(server);

        int timeout_ms = 3000;  // how long time wait per scan
        while (!should_stop.load(std::memory_order_acquire)) {
            // wait for events
            int pollResult = poll(pollfds.data(), pollfds.size(), timeout_ms);

            if (pollResult < 0) {
                std::cerr << "[err]: broken poll\n";
                // todo: throw exeption
                continue;
            } else if (pollResult == 0) {
                #ifdef ADDITIONAL_POLLMSG
                std::clog << "[log]: poll detects no changes\n";
                #endif
                continue;
            }

            // check server socket
            if (pollfds[0].revents & POLLIN) {
                // create for client new socket
                int fd = accept(server_sockguard.getSock(), nullptr, nullptr);
                
                if (fd > 0) {
                    pollfd client{};
                    client.fd = fd;
                    client.events = POLLIN;
                    pollfds.push_back(client);
                    std::clog << "[log]: created new socket\n";
                } else {
                    std::cerr << "[err]: can't create socket\n";
                }
            }
            
            // check other sockets
            for (int i = 1; i < pollfds.size(); i++) {
                if (pollfds[i].revents & POLLIN) {
                    std::clog << "[log]: client socket got msg\n";
                    
                    ssize_t buf_size = read(pollfds[i].fd, buffer, BUFFER_SIZE - 1);

                    if (buf_size > 0) {
                        // read data
                        std::cout << "[msg]:\n" << buffer << std::endl;
                    } else if (buf_size == 0) {
                        // close socket
                        std::clog << "[log]: EOF. Connection stoped\n";
                        close(pollfds[i].fd);
                        pollfds.erase(pollfds.begin() + i);
                    } else {
                        // todo: throw error
                    }
                }
            }
        }

        // close all fds
        for (auto item : pollfds) {
            close(item.fd);
            item.fd = 0;    // actually need for safety close socket in SockGuard
        }
        pollfds.clear();
    }

    void startup() {
        try {
            server_sockguard.initAsReciever();
            std::clog << "[log]: server socket initialized\n";
            resive();
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
};

class Client
{
private:
    SockGuard client_sockguard;
    static const size_t BUFFER_SIZE = 128;
    char buffer[BUFFER_SIZE];

private:
    void sendmsg() {
        std::string msg;

        while (true) {
            std::getline(std::cin, msg);   // get input
            if (std::string_view(msg) == EXIT_CODE) {
                std::clog << "[log] App is stoping\n";
                break;
            } else {
                std::cerr << "[warn] Unknown command:\n";
            }

            size_t text_max_len = BUFFER_SIZE - 2;   // keep place for '\0'
            size_t copied = 0;

            while (copied < msg.size()) {
                size_t to_copy = std::min(msg.size(), text_max_len);
                msg.copy(buffer, to_copy, copied);
                buffer[to_copy + 1] = '\0';
                copied += to_copy;

                std::cout << buffer << std::endl;
                if (client_sockguard.getSock() > 0)
                    send(client_sockguard.getSock(), buffer, sizeof(buffer) - 1, 0);
            }
        }
    }

public:
    void startup() {
        try {
            client_sockguard.initAsSender();
            std::clog << "[log]: client socket initialized" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        sendmsg();
    }
};

// ##### functions section #####
void runServerApp() {
    auto startupServer = [&]() {
        Server server;
        server.startup();
    };
    std::thread th_server(startupServer);

    // choice when to stop the server and close the app
    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (std::string_view(command) == EXIT_CODE) {
            std::clog << "[log] App is stoping\n";
            break;
        } else {
            std::cerr << "[warn] Unknown command:\n";
        }
    }
    should_stop.store(true, std::memory_order_release);

    if (th_server.joinable())
        th_server.join();
}

void runClientApp() {
    Client client;
    client.startup();
}

// ########## MAIN SECTION ##########
int main(const int args, const char* argv[]) {
    
    if (argv[1]) {
        std::string_view param = argv[1];
        if (param == "-s") {
            runServerApp();
        } else if (param == "-c") {
            runClientApp();
        } else {
            std::cerr << "[err] Wrong flag\n";
        }
        std::cout << "[log] App successfully shutdown\n";
    } else {
        std::cout << "[Tip]: Try to use custom [-s] or [-c] flag\n";
    }
}