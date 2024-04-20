#ifndef DNS_REVERSE_PROXY_DNS_HPP
#define DNS_REVERSE_PROXY_DNS_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

#include <ldns/ldns.h>

#include "dns_resolver.hpp"

class DNS {
public:
    class Packet {
    public:
        Packet(const uint8_t *data, size_t len);

        virtual ~Packet();

        ldns_pkt *_pkt;
    };

    static size_t createResponse(uint8_t *buffer, Packet *dnsPacket);

    static std::vector<std::string> resolveQuestions(ldns_pkt *dnsPacket);
};

#endif //DNS_REVERSE_PROXY_DNS_HPP
