#ifndef DNS_REVERSE_PROXY_UDP_H
#define DNS_REVERSE_PROXY_UDP_H

#include "socket.hpp"

class UDP : public Socket {
public:
    UDP(int addr, int port);
};


#endif //DNS_REVERSE_PROXY_UDP_H
