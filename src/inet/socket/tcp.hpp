#ifndef DNS_REVERSE_PROXY_TCP_HPP
#define DNS_REVERSE_PROXY_TCP_HPP

#include "socket.hpp"

class TCPClient : public SocketClient {
public:
    TCPClient(const sockaddr_in &addr, socklen_t addrLen, int fd);

    ssize_t send(void *buff, ssize_t len) override;

    int close();
};

using NewConnectionCallback = std::function<void(TCPClient *)>;
using ConnectionClosedCallback = std::function<void(TCPClient *)>;
using OnDataCallback = std::function<void(TCPClient *, void *data, ssize_t len)>;

class TCP : public Socket {
public:
    TCP(int addr, int port);
    ~TCP();

    void listen(int backlog = _SOCK_DEFAULT_BACKLOG);

    void setNewConnectionCallback(NewConnectionCallback callback);

    void setConnectionClosedCallback(ConnectionClosedCallback callback);

    void setOnDataCallback(OnDataCallback callback);

protected:
    int epollFd;
    std::vector<TCPClient *> connections;
    NewConnectionCallback newConnectionCallback;
    ConnectionClosedCallback connectionClosedCallback;
    OnDataCallback onDataCallback;

    void handleNewConnection();

    void handleData(TCPClient *client);
};

#endif
