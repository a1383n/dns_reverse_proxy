#ifndef DNS_REVERSE_PROXY_DNS_PACKET_H
#define DNS_REVERSE_PROXY_DNS_PACKET_H

#include "dns.hpp"

struct dns_question_msg_t {
    std::string qname;
    uint16_t qtype;
    uint16_t qclass;
};

struct dns_header_msg_t {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

class DNSPacket {
public:
    DNSPacket(uint8_t *buff, unsigned long len);

    static ssize_t generateResponse(uint8_t *buff, const DNSPacket &dnsPacket, const std::string &ip);

    struct dns_header_msg_t header;
    struct std::vector<dns_question_msg_t> questions;
};

#endif //DNS_REVERSE_PROXY_DNS_PACKET_H
