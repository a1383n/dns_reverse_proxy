#ifndef DNS_REVERSE_PROXY_UDP_H
#define DNS_REVERSE_PROXY_UDP_H

#include "socket.hpp"

class UDPClient : public SocketClient {
public:
    UDPClient(const sockaddr_in addr, socklen_t addrLen, int fd);

    ssize_t send(void *buff, ssize_t len) override;
};

class UDP : public Socket {
public:
    UDP(int addr, int port);

    ~UDP();

    void receive();

    void setOnDataCallback(std::function<void(UDPClient *, void *, ssize_t)> callback);

protected:
    int epollFd;
    std::function<void(UDPClient *, void *, ssize_t)> onDataCallback;
};

#endif //DNS_REVERSE_PROXY_UDP_H
