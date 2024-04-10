#include "udp.hpp"

UDP::UDP(int addr, int port) : Socket(SOCK_DGRAM, addr, port) {

}

UDP::~UDP() {

}

void UDP::receive() {
    for (;;) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        char buffer[buffer_len];
        ssize_t bytesReceived = recvfrom(socketFd, buffer, sizeof(buffer), 0, (struct sockaddr *) &clientAddr,
                                         &clientAddrLen);
        UDPClient *client = new UDPClient(clientAddr, clientAddrLen, socketFd);


        if (bytesReceived == -1) {
            error("Error receiving data", false);
        } else {
            if (onDataCallback) {
                onDataCallback(client, buffer, bytesReceived);
                delete client;
            }
        }
    }
}

void UDP::setOnDataCallback(std::function<void(UDPClient *, void *, ssize_t)> callback) {
    onDataCallback = std::move(callback);
}

UDPClient::UDPClient(const sockaddr_in addr, socklen_t addrLen, int fd) : SocketClient(addr, addrLen, fd) {}

ssize_t UDPClient::send(void *buff, ssize_t len) {
    return ::sendto(fd, buff, len, 0, (const struct sockaddr *) &addr, addrLen);
}
