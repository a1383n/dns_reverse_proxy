#include <iostream>

#include "inet/socket/tcp.hpp"
#include "inet/socket/udp.hpp"
#include "inet/dns/dns.hpp"

#include "config/config.h"
#include "utility.hpp"

#include <csignal>
#include <thread>
#include <gflags/gflags.h>


DEFINE_string(endpoint, "", "DNS Resolver Endpoint URL");
DEFINE_int32(port, 53, "Listen port");
DEFINE_bool(debug, (bool) false, "Debug flag");

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

    printf("TCP server start listing on ::%d\n", FLAGS_port);

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

    printf("UDP server start listing on ::%d\n", FLAGS_port);
}

int main(int argc, char *argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    gflags::SetVersionString("1.0.0");

    std::string endpoint_url;

    if (!FLAGS_endpoint.empty()) {
        endpoint_url = FLAGS_endpoint;
    } else {
        char *env_endpoint = std::getenv("ENDPOINT_URL");
        if (env_endpoint != nullptr) {
            endpoint_url = env_endpoint;
        } else {
            std::cerr << "Endpoint URL not provided via command line or environment variable" << std::endl;
            return 1;
        }
    }

    signal(SIGABRT, terminate);
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);


    initializeTcpSocket(FLAGS_port);
    initializeUdpSocket(FLAGS_port);

    Config::setHttpResolverUrl(endpoint_url);
    Config::setDebugMode(FLAGS_debug);

    auto handler = [](SocketClient *client, void *buff, size_t len) {
        uint8_t b[4096];
        try {
            auto *packet = new DNS::Packet(client, (const uint8_t *) buff, len);
            auto l = DNS::createResponse(packet, b);
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
