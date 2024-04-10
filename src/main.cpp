#include <iostream>

#include "inet/tcp.hpp"

#include <csignal>
#include <thread>
#include <string.h>

static TCP *tcp;

void terminate(int s) {
    printf("Terminating with signal %d...\n", s);
    delete tcp;
    exit(0);
}

int main() {
    signal(SIGABRT, terminate);
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);

    tcp = new TCP(INADDR_ANY, 9000);
    tcp->bind();

    tcp->setNewConnectionCallback([](TCPClient *client) {
        printf("New client connected: %s:%d\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
        fflush(stdout);
    });

    tcp->setConnectionClosedCallback([](TCPClient *client) {
        printf("Client %s:%d disconnected\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
        fflush(stdout);
    });

    tcp->setOnDataCallback([](TCPClient *client, void *buff, size_t len) {
        const char *s = "HTTP/1.1 200 OK\n"
                        "Content-Length: 53\n"
                        "Content-Type: text/html\n"
                        "Connection: Closed\n\n"
                        "<html>\n"
                        "<body>\n"
                        "<h1>Hello, World!</h1>\n"
                        "</body>\n"
                        "</html>\n";

        send(client->fd, s, strlen(s), 0);
        shutdown(client->fd, SHUT_RDWR);
    });

    // Listen on separate thread
    std::thread t(&TCP::listen, tcp, _SOCK_DEFAULT_BACKLOG);
    t.detach();

    getchar();
    terminate(0);
}
