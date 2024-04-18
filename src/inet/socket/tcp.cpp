#include "tcp.hpp"
#include <sys/epoll.h>
#include <unistd.h>

TCP::TCP(int addr, int port) : Socket(SOCK_STREAM, addr, port) {
    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        error("Failed to create epoll");
    }
}

void TCP::listen(int backlog) {
    if (::listen(socketFd, backlog) != 0) {
        error("Failed to listen socket");
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = socketFd;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, socketFd, &event) == -1) {
        error("Failed to add socket to epoll");
    }

    struct epoll_event events[_SOCK_MAX_EVENTS];

    for (;;) {
        int nfds = epoll_wait(epollFd, events, _SOCK_MAX_EVENTS, -1);
        if (nfds == -1) {
            error("Error in epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == socketFd) {
                handleNewConnection();
            } else {
                for (auto &client: connections) {
                    if (events[i].data.fd == client->fd) {
                        handleData(client);
                    }
                }
            }
        }
    }
}

void TCP::handleNewConnection() {
    int clientFd;
    struct sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);

    if ((clientFd = accept(socketFd, (struct sockaddr *) &clientAddr, &clientAddrLen)) < 0) {
        error("Failed to accept connection", false);
        return;
    }

    TCPClient *client = new TCPClient(clientAddr, clientAddrLen, clientFd);
    connections.push_back(client);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = clientFd;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
        error("Failed to add client socket to epoll");
        close(clientFd);
        delete client;
    }

    if (newConnectionCallback) {
        newConnectionCallback(client);
    }
}

void TCP::handleData(TCPClient *client) {
    char buffer[buffer_len];
    ssize_t bytesRead = recv(client->fd, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        // Error or client closed connection
        if (connectionClosedCallback) {
            connectionClosedCallback(client);
        }
        epoll_ctl(epollFd, EPOLL_CTL_DEL, client->fd, nullptr);
        close(client->fd);
        connections.erase(std::remove(connections.begin(), connections.end(), client), connections.end());
        delete client;
    } else {
        // Process received data
        if (onDataCallback) {
            onDataCallback(client, buffer, bytesRead);
        }
    }
}

void TCP::setNewConnectionCallback(NewConnectionCallback callback) {
    newConnectionCallback = std::move(callback);
}

void TCP::setConnectionClosedCallback(ConnectionClosedCallback callback) {
    connectionClosedCallback = std::move(callback);
}

void TCP::setOnDataCallback(OnDataCallback callback) {
    onDataCallback = std::move(callback);
}

TCP::~TCP() {
    for (auto &client: connections) {
        epoll_ctl(epollFd, EPOLL_CTL_DEL, client->fd, nullptr);
        close(client->fd);
        delete client;
    }
    close(epollFd);
}

TCPClient::TCPClient(const sockaddr_in &addr, socklen_t addrLen, int fd) : SocketClient(addr, addrLen, fd) {}

ssize_t TCPClient::send(void *buff, ssize_t len) {
    return ::send(fd, buff, len, MSG_CONFIRM);
}
