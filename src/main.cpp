#include <iostream>

#include "inet/tcp.hpp"

#include <arpa/inet.h>
#include <csignal>
#include <thread>

static TCP *tcp;

void terminate(int s) {
    printf("Terminating with signal %d...\n", s);
    tcp->terminate();
    delete tcp;
    exit(0);
}

int main() {
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);

    try {
        tcp = new TCP(INADDR_ANY, 3000);
        tcp->initialize();
    } catch (std::runtime_error &e) {
        perror(e.what());
        delete tcp;
        return 1;
    }

    tcp->setNewConnectionCallback([](struct sock_conn_t *client) {
        printf("New client connected: %s:%d\n", inet_ntoa(client->clientAddr.sin_addr),
               (int) ntohs(client->clientAddr.sin_port));
    });

    std::thread t(&TCP::start, tcp);
    t.detach();

    getchar();
    terminate(0);
}
