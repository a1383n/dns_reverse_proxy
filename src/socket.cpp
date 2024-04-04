#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

#include "socket.h"

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

Socket::~Socket() noexcept(false) {
    if (socketFd > 0) {
        if (close(socketFd) != 0) {
            error("Failed to close socket");
        }
    }
}

void Socket::initilize(int backlog) {
    if (bind(socketFd, (const struct sockaddr*)&s_addr, addr_len) != 0) {
        error("Failed to bind socket");
    }

    if (listen(socketFd, backlog) != 0) {
        error("Failed to listen socket");
    }
}

void Socket::error(const char *s) {
    throw std::runtime_error(s);
}

#ifdef __DEBUG_MODE
static void debug(const char *s) {
    //TODO: Print some debug information
}
#endif