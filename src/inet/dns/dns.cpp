#include <cstring>
#include "dns.hpp"

DNS::Packet::Packet(SocketClient *socketClient, const uint8_t *data, size_t len) : socketClient(socketClient) {
    ldns_enum_status s = ldns_wire2pkt(&_pkt, data, len);

    if (s != LDNS_STATUS_OK) {
        throw std::runtime_error("Invalid status");
    }
}

std::vector<std::string> DNS::resolveQuestions(DNS::Packet *packet) {
    ldns_pkt *dnsPacket = packet->_pkt;
    std::vector<std::string> resolvedIPs;
    HttpDNSResolver resolver = HttpDNSResolver();

    ldns_rr_list *questions = ldns_pkt_question(dnsPacket);
    if (questions) {
        size_t questionCount = ldns_rr_list_rr_count(questions);
        for (size_t i = 0; i < questionCount; ++i) {
            ldns_rr *questionRR = ldns_rr_list_rr(questions, i);
            if (questionRR) {
                // Extract domain name from the question RR
                char *qname = ldns_rr_owner(questionRR) ? ldns_rdf2str(ldns_rr_owner(questionRR)) : nullptr;
                if (qname) {
                    std::string qnameStr(qname);
                    std::string ip;
                    int resolveStatus = resolver.resolve(inet_ntoa(packet->socketClient->addr.sin_addr), qnameStr, &ip);
                    if (resolveStatus == 0) {
                        resolvedIPs.push_back(ip);
                    }
                    LDNS_FREE(qname); // Free allocated memory for qname
                }
            }
        }
    }

    return resolvedIPs;
}

ssize_t DNS::createResponse(DNS::Packet *packet, uint8_t *buffer) {
    ldns_pkt *dnsPacket = packet->_pkt;
    ldns_pkt *responsePacket = ldns_pkt_new();
    if (!responsePacket) {
        return -1;
    }

    ldns_pkt_set_id(responsePacket, ldns_pkt_id(dnsPacket));
    ldns_pkt_set_qr(responsePacket, 1);
    ldns_pkt_set_aa(responsePacket, 1);

    ldns_rr_list *questions = ldns_pkt_question(dnsPacket);
    if (questions) {
        size_t questionCount = ldns_rr_list_rr_count(questions);
        for (size_t i = 0; i < questionCount; ++i) {
            ldns_rr *questionRR = ldns_rr_list_rr(questions, i);
            if (questionRR) {
                ldns_rr *questionCopy = ldns_rr_clone(questionRR);
                ldns_pkt_push_rr(responsePacket, LDNS_SECTION_QUESTION, questionCopy);
            }
        }
    }

    auto resolvedIPs = DNS::resolveQuestions(packet);
    for (const std::string &ip: resolvedIPs) {
        ldns_rr *answerRR = ldns_rr_new();
        if (questions && ldns_rr_list_rr_count(questions) > 0) {
            ldns_rr_set_owner(answerRR, ldns_rr_owner(ldns_rr_list_rr(questions, 0)));
        } else {
            // Handle case when no questions are present in the packet
            ldns_rdf *origin = ldns_dname_new_frm_str(".");
            ldns_rr_set_owner(answerRR, origin);
            ldns_rdf_deep_free(origin);
        }
        ldns_rr_set_class(answerRR, LDNS_RR_CLASS_IN);
        ldns_rr_set_ttl(answerRR, 120);
        ldns_rr_set_type(answerRR, LDNS_RR_TYPE_A);
        ldns_rr_push_rdf(answerRR, ldns_rdf_new_frm_str(LDNS_RDF_TYPE_A, ip.c_str()));
        ldns_pkt_push_rr(responsePacket, LDNS_SECTION_ANSWER, answerRR);
    }

    uint8_t *responseWire;
    size_t responseWireLen;

    //TODO: Free pointers (Carefully do this)
    //TODO: error handling
    ldns_status responseStatus = ldns_pkt2wire(&responseWire, responsePacket, &responseWireLen);

    if (responseStatus != LDNS_STATUS_OK) {
        return -1;
    }

    std::memcpy(buffer, responseWire, responseWireLen);

    return responseWireLen;
}
