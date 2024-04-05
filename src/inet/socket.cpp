#include <stdexcept>
#include <utility>
#include <sys/socket.h>
#include <unistd.h>

#include "socket.hpp"

//#define __DEBUG_MODE

Socket::Socket(int type, int addr, int port) {
    socketFd = socket(AF_INET, type, 0);
    if (socketFd < 0) {
        error("Failed to open socket");
    }

    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(addr);
    s_addr.sin_port = htons(port);
    addr_len = sizeof(s_addr);
}

Socket::~Socket() {
    if (socketFd > 0) {
        if (close(socketFd) != 0) {
            error("Failed to close socket", false);
        }
    }
}

void Socket::initialize(int backlog) {
    if (bind(socketFd, (const struct sockaddr *) &s_addr, addr_len) != 0) {
        error("Failed to bind socket");
    }

    if (listen(socketFd, backlog) != 0) {
        error("Failed to listen socket");
    }
}

void Socket::error(const char *s, bool thw) {
    if (thw) {
        throw std::runtime_error(s);
    } else {
        perror(s);
    }
}

void Socket::setOptions(int level, int optname) {
    if (setsockopt(socketFd, level, optname, &option, sizeof(option)) != 0) {
        error("Failed to set option");
    }
}

void Socket::setNewConnectionCallback(NewConnectionCallback callback) {
    newConnectionCallback = std::move(callback);
}

void Socket::start() {
    for (;;) {
        int clientFd;
        struct sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);

        if ((clientFd = accept(socketFd, (struct sockaddr *) &clientAddr, &clientAddrLen)) < 0) {
            error("failed to accept connection", false);
            break;
        }

        struct sock_conn_t conn{
                .clientFd = clientFd,
                .clientAddr = clientAddr,
                .clientAddrLen = clientAddrLen
        };

        connections.push_back(conn);
        newConnectionCallback(&conn);
    }
}

void Socket::terminate() {
    for (auto &connection: connections) {
        close(connection.clientFd);
    }
}

#ifdef __DEBUG_MODE
static void debug(const char *s) {
    //TODO: Print some debug information
}
#endif