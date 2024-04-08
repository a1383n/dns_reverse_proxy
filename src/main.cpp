#include <iostream>

#include "inet/tcp.hpp"

#include <csignal>
#include <thread>

static TCP *tcp;

void terminate(int s) {
    printf("Terminating with signal %d...\n", s);
    delete tcp;
    exit(0);
}

int main() {
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);

    tcp = new TCP(INADDR_ANY, 3000);
    tcp->bind();

    auto a = [](TCPClient *client) {
        printf("New client connected: %s:%d\n", inet_ntoa(client->addr.sin_addr),
               (int) ntohs(client->addr.sin_port));
    };

    tcp->setNewConnectionCallback((void *(*)(TCPClient *)) &a);

    std::thread t(&TCP::listen, tcp, _SOCK_DEFAULT_BACKLOG);
    t.detach();

    getchar();
    terminate(0);
}
