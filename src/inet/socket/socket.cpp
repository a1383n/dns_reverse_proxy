#include <stdexcept>
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

void Socket::bind() {
    if (::bind(socketFd, (const struct sockaddr *) &s_addr, addr_len) != 0) {
        error("Failed to bind socket");
    }
}

void Socket::error(const char *s, bool thw) {
    if (thw) {
        perror(s);
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

void Socket::setReadBuffer(size_t s) {
    this->buffer_len = s;
}

#ifdef __DEBUG_MODE
static void debug(const char *s) {
    //TODO: Print some debug information
}
#endif

SocketClient::SocketClient(const sockaddr_in addr, socklen_t addrLen, int fd) : addr(addr), addrLen(addrLen), fd(fd) {}
