#ifndef DNS_REVERSE_PROXY_DNS_HPP
#define DNS_REVERSE_PROXY_DNS_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

#include <ldns/ldns.h>

#include "../socket/socket.hpp"
#include "dns_resolver.hpp"

class DNS {
public:
    class Packet {
    public:
        Packet(SocketClient *socketClient, const uint8_t *data, size_t len);

        ldns_pkt *_pkt;
        SocketClient *socketClient;
    };

    static ssize_t createResponse(DNS::Packet *packet, uint8_t *buffer);

    static std::vector<std::string> resolveQuestions(DNS::Packet *packet);
};

#endif //DNS_REVERSE_PROXY_DNS_HPP
