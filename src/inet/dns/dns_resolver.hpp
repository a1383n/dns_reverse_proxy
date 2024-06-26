#ifndef DNS_REVERSE_PROXY_DNS_RESOLVER_H
#define DNS_REVERSE_PROXY_DNS_RESOLVER_H

#include <string>

class DNSResolver {
public:
    virtual int resolve(std::string clientIp, std::string qname, std::string *ip) = 0;
};

class HttpDNSResolver : public DNSResolver {
public:
    int resolve(std::string clientIp, std::string qname, std::string *ip) override;
};


#endif //DNS_REVERSE_PROXY_DNS_RESOLVER_H
