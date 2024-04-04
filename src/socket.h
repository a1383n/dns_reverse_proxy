#ifndef DNS_REVERSE_PROXY_SOCKET_H
#define DNS_REVERSE_PROXY_SOCKET_H

#define _SOCK_DEFAULT_BACKLOG 64

#include <netinet/in.h>

class Socket {
public:
    explicit Socket(int type, int addr, int port);
    ~Socket() noexcept(false);
    virtual void initilize(int backlog = _SOCK_DEFAULT_BACKLOG);

private:
    int socketFd;
    struct sockaddr_in s_addr {};
    socklen_t addr_len;
    static void error(const char *s);

#ifdef __DEBUG_MODE
    static void debug(const char *s);
#endif
};


#endif //DNS_REVERSE_PROXY_SOCKET_H
