#ifndef DNS_REVERSE_PROXY_SOCKET_HPP
#define DNS_REVERSE_PROXY_SOCKET_HPP

#define _SOCK_DEFAULT_BACKLOG 64

#include <netinet/in.h>
#include <vector>
#include <functional>

struct sock_conn_t {
    int clientFd;
    sockaddr_in clientAddr;
    socklen_t clientAddrLen;
};

using NewConnectionCallback = std::function<void(struct sock_conn_t *)>;


class Socket {
public:
    explicit Socket(int type, int addr, int port);

    ~Socket();

    void setOptions(int level, int optname);

    void initialize(int backlog = _SOCK_DEFAULT_BACKLOG);

    void terminate();

    void start();

    void setNewConnectionCallback(NewConnectionCallback callback);

private:
    int socketFd, option;
    struct sockaddr_in s_addr {};
    socklen_t addr_len;
    NewConnectionCallback newConnectionCallback;
    std::vector<sock_conn_t> connections = {};

    static void error(const char *s, bool thw = true);

#ifdef __DEBUG_MODE
    static void debug(const char *s);
#endif
};


#endif //DNS_REVERSE_PROXY_SOCKET_HPP
