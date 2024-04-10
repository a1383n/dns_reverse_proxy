#ifndef DNS_REVERSE_PROXY_TCP_HPP
#define DNS_REVERSE_PROXY_TCP_HPP

#include "socket.hpp"


class TCPClient : public SocketClient {
public:
    TCPClient(const sockaddr_in &addr, socklen_t addrLen, int fd);

    int fd;
};

using NewConnectionCallback = std::function<void(TCPClient *)>;

class TCP : public Socket {
public:
    TCP(int addr, int port);

    ~TCP();

    void listen(int backlog = _SOCK_DEFAULT_BACKLOG);

    void setNewConnectionCallback(NewConnectionCallback callback);

protected:
    std::vector<TCPClient *> connections = {};
    NewConnectionCallback newConnectionCallback;
};

#endif
