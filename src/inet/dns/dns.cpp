#include "dns.hpp"

DNS::Packet::Packet(const uint8_t *data, size_t len) {
    ldns_enum_status s = ldns_wire2pkt(&_pkt, data, len);

    if (s != LDNS_STATUS_OK) {
        throw std::runtime_error("wire2pkt failed");
    }
}

DNS::Packet::~Packet() {
    ldns_pkt_free(_pkt);
}

std::vector<std::string> DNS::resolveQuestions(ldns_pkt *dnsPacket) {
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
                    std::string ip = resolver.resolve(qnameStr);
                    resolvedIPs.push_back(ip);
                    free(qname); // Free allocated memory for qname
                }
            }
        }
    }
//    ldns_rr_list_deep_free(questions);

    return resolvedIPs;
}

size_t DNS::createResponse(uint8_t *buffer, Packet *dnsPacket) {
    ldns_pkt *responsePacket = ldns_pkt_new();
    if (!responsePacket) {
        return -1;
    }

    ldns_pkt_set_id(responsePacket, ldns_pkt_id(dnsPacket->_pkt));
    ldns_pkt_set_qr(responsePacket, true);
    ldns_pkt_set_aa(responsePacket, true);

    ldns_rr_list *questions = ldns_pkt_question(dnsPacket->_pkt);
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

    auto resolvedIPs = DNS::resolveQuestions(dnsPacket->_pkt);
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
        ldns_rr_set_ttl(answerRR, 120); //TODO: Dynamic TTL
        ldns_rr_set_type(answerRR, LDNS_RR_TYPE_A);
        ldns_rr_push_rdf(answerRR, ldns_rdf_new_frm_str(LDNS_RDF_TYPE_A, ip.c_str()));
        ldns_pkt_push_rr(responsePacket, LDNS_SECTION_ANSWER, answerRR);
    }

    size_t responseWireLen;

    ldns_rr_list_free(questions);
    ldns_status responseStatus = ldns_pkt2wire(&buffer, responsePacket, &responseWireLen);

    if (responseStatus != LDNS_STATUS_OK) {
        throw std::runtime_error("pkt2wire failed");
    }

    return responseWireLen;
}
