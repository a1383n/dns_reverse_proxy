#ifndef DNS_REVERSE_PROXY_TCP_HPP
#define DNS_REVERSE_PROXY_TCP_HPP

#include "socket.hpp"

class TCP : public Socket {
public:
    TCP(int addr, int port);
};


#endif
