#ifndef DNS_REVERSE_PROXY_DNS_RESOLVER_H
#define DNS_REVERSE_PROXY_DNS_RESOLVER_H

#include <string>
#include <vector>

class DNSResolver {
public:
    virtual std::string resolve(std::string qname) = 0;

    virtual std::vector<std::string> resolve(std::vector<std::string> qname) = 0;
};

class HttpDNSResolver : public DNSResolver {
public:
    std::string resolve(std::string qname) override;

    std::vector<std::string> resolve(std::vector<std::string> qname) override;
};


#endif //DNS_REVERSE_PROXY_DNS_RESOLVER_H
