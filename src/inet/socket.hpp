#ifndef DNS_REVERSE_PROXY_SOCKET_HPP
#define DNS_REVERSE_PROXY_SOCKET_HPP

#define _SOCK_DEFAULT_BACKLOG 64
#define SOCK_DEFAULT_READ_BUFF 4096

#include <netinet/in.h>
#include <arpa/inet.h> // inet_ntoa(),
#include <vector>
#include <functional>

class Socket {
public:
    explicit Socket(int type, int addr, int port);

    ~Socket();

    void setOptions(int level, int optname);

    void bind();


protected:
    int socketFd, option;
    struct sockaddr_in s_addr {};
    socklen_t addr_len;

    static void error(const char *s, bool thw = true);

#ifdef __DEBUG_MODE
    static void debug(const char *s);
#endif
};

class SocketClient {
public:
    SocketClient(const sockaddr_in &addr, socklen_t addrLen);

    sockaddr_in addr;
    socklen_t addrLen;
};

#endif //DNS_REVERSE_PROXY_SOCKET_HPP
