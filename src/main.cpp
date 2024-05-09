#include <iostream>

#include "inet/socket/tcp.hpp"
#include "inet/socket/udp.hpp"
#include "inet/dns/dns.hpp"

#include "config/config.h"
#include "utility.hpp"

#include <csignal>
#include <thread>
#include <cstring>

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

    Config::setHttpResolverUrl("http://127.0.0.1:8000/api/resolve?qname=");

    auto handler = [](SocketClient *client, void *buff, size_t len) {
        uint8_t b[4096];
        try {
            auto *packet = new DNS::Packet((const uint8_t *) buff, len);
            auto l = DNS::createResponse(b, packet->_pkt);
            client->send(b, l);
            delete packet;
        } catch (std::runtime_error &e) {
            if (instanceof<TCPClient>(client)) {
                ((TCPClient *) client)->send(b, len);
            }

            printf("E: %s\n", e.what());
            fflush(stdout);
        }
    };

    tcp->setOnDataCallback(handler);
    udp->setOnDataCallback(handler);

    // Listen on separate thread
    tcpTread = std::thread(&TCP::listen, tcp, _SOCK_DEFAULT_BACKLOG);
    udpThread = std::thread(&UDP::receive, udp);

    tcpTread.detach();
    udpThread.detach();

    getchar();
    terminate(EXIT_SUCCESS);
}
