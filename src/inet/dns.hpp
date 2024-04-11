#ifndef DNS_REVERSE_PROXY_DNS_HPP
#define DNS_REVERSE_PROXY_DNS_HPP

#include <cstdint>
#include <vector>
#include <string>

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

struct dns_packet_t {
    struct dns_header_msg_t header;
    struct std::vector<dns_question_msg_t> questions;
};

class DNS {
public:
    static struct dns_packet_t parseQueryPacket(uint8_t *buff, unsigned long len);
};


#endif //DNS_REVERSE_PROXY_DNS_HPP
