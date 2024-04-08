#include "tcp.hpp"
#include <csignal>

TCP::TCP(int addr, int port) : Socket(SOCK_STREAM, addr, port) {
    //
}

void TCP::listen(int backlog) {
    if (::listen(socketFd, backlog) != 0) {
        error("Failed to listen socket");
    }

    for (;;) {
        int clientFd;
        struct sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);

        if ((clientFd = accept(socketFd, (struct sockaddr *) &clientAddr, &clientAddrLen)) < 0) {
            error("failed to accept connection", false);
            break;
        }

        TCPClient client = TCPClient(clientAddr, clientAddrLen, clientFd);

        connections.push_back(client);
        ((void (*)(TCPClient)) newConnectionCallback)(client);
    }
}


void TCP::setNewConnectionCallback(void *(*callback)(TCPClient *)) {
    newConnectionCallback = (void *) callback;
}

TCP::~TCP() {
    for (auto &connection: connections) {
        close(connection.fd);
    }
}

TCPClient::TCPClient(const sockaddr_in &addr, socklen_t addrLen, int fd) : SocketClient(addr, addrLen), fd(fd) {}
