#include <iostream>

#include "inet/tcp.hpp"
#include "inet/udp.hpp"
#include "inet/dns.hpp"

#include <csignal>
#include <thread>
#include <string.h>

static TCP *tcp;
static UDP *udp;
std::thread tcpTread;
std::thread udpThread;

void terminate(int s) {
    printf("Terminating with signal %d...\n", s);
    delete tcp;
    delete udp;
    exit(EXIT_SUCCESS);
}

void initializeTcpSocket(int port) {
    tcp = new TCP(INADDR_ANY, port);
    tcp->bind();

    tcp->setNewConnectionCallback([](TCPClient *client) {
        printf("New client connected: %s:%d\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
        fflush(stdout);
    });

    tcp->setConnectionClosedCallback([](TCPClient *client) {
        printf("Client %s:%d disconnected\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
        fflush(stdout);
    });
}

void initializeUdpSocket(int port) {
    udp = new UDP(INADDR_ANY, port);
    udp->bind();
}

int main() {
    signal(SIGABRT, terminate);
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);

    initializeTcpSocket(3000);
    initializeUdpSocket(3000);

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

        client->send((void *) s, strlen(s));
        shutdown(client->fd, SHUT_RDWR);
    });
    udp->setOnDataCallback([](UDPClient *client, void *buff, ssize_t len) {
        struct dns_packet_t dnsPacket = DNS::parseQueryPacket((uint8_t *) buff, len);

        printf("%s\n", dnsPacket.questions[0].qname.c_str());
        fflush(stdout);

//        client->send(buff, len);
    });

    // Listen on separate thread
    tcpTread = std::thread(&TCP::listen, tcp, _SOCK_DEFAULT_BACKLOG);
    udpThread = std::thread(&UDP::receive, udp);

    tcpTread.detach();
    udpThread.detach();

    getchar();
    terminate(EXIT_SUCCESS);
}
