#include "dns_packet.hpp"

#include <stdexcept>
#include <cstring>
#include <netinet/in.h>

#define U16S(_PAYLOAD, _INDEX) (uint8_t)((((uint8_t*)(_PAYLOAD))[_INDEX] << 8) + ((uint8_t*)(_PAYLOAD))[_INDEX + 1])
#define U16B(_BUFF, _INDEX, _VALUE) *((uint8_t *) (_BUFF + _INDEX)) = _VALUE

DNSPacket::DNSPacket(uint8_t *buff, unsigned long len) {
    if (len < sizeof(dns_header_msg_t)) {
        throw std::runtime_error("Invalid packet size");
    }

    dns_header_msg_t headerMsg = {
            .id = U16S(buff, 0),
            .flags = U16S(buff, 2),
            .qdcount = U16S(buff, 4),
            .ancount = U16S(buff, 6),
            .nscount = U16S(buff, 8),
            .arcount = U16S(buff, 10),
    };

    if (headerMsg.qdcount == 0) {
        throw std::runtime_error("No queries found");
    }

    int queryOffset = sizeof(dns_header_msg_t);
    std::vector<dns_question_msg_t> questions;

    for (int i = 0; i < headerMsg.qdcount; ++i) {
        std::string qname;
        int labelLen = 0;
        int pos = queryOffset;

        while (buff[pos] != 0 && pos < len) {
            labelLen = buff[pos++];
            if ((labelLen & 0xc0) == 0xc0) {
                // Handle compressed name
                break;
            }
            if (qname.length() > 0) qname += ".";
            for (int j = 0; j < labelLen; ++j) {
                qname += buff[pos++];
            }
        }
        queryOffset = pos + 5;  // Move past the label length byte, type, and class fields

        if (labelLen == 0 || pos >= len) {
            throw std::runtime_error("Invalid query format");
        }

        struct dns_question_msg_t questionMsg{
                .qname = qname,
                .qtype = U16S(buff, queryOffset - 4),
                .qclass = U16S(buff, queryOffset - 2)
        };

        questions.push_back(questionMsg);
    }

    this->header = headerMsg;
    this->questions = questions;
}

ssize_t DNSPacket::generateResponse(uint8_t *buff, const DNSPacket &dnsPacket, const std::string &ip) {
    ssize_t s = 0;

    U16B(buff, s, htons(dnsPacket.header.id));
    s += 2;

    U16B(buff, s, 0x8180); // Sample flags (response, no error)
    s += 2;

    U16B(buff, s, dnsPacket.header.qdcount);
    s += 2;

    U16B(buff, s, 1);
    s += 2;

    // Skip NSCOUNT and ARCOUNT (set to 0 for now)
    U16B(buff, s, 0);
    s += 2;
    U16B(buff, s, 0);
    s += 2;

    // Copy the question section from the DNS query packet to the response
    std::memcpy(buff + s, dnsPacket.questions[0].qname.c_str(), dnsPacket.questions[0].qname.length());
    s += dnsPacket.questions[0].qname.length();

    U16B(buff, s, 1); // QTYPE (A record)
    s += 2;
    U16B(buff, s, 1); // QCLASS (IN)
    s += 2;

    // Add the answer section with the provided IP address
    std::string answer = "\xC0\x0C\x00\x01\x00\x01\x00\x00\x00\x3C\x00\x04";
    answer += ip;
    std::memcpy(buff + s, answer.c_str(), answer.length());
    s += answer.length();

    return s;
}