#ifndef DNS_REVERSE_PROXY_CONFIG_H
#define DNS_REVERSE_PROXY_CONFIG_H

#include <string>

class Config {
public:
    static std::string getHttpResolverUrl();

    static void setHttpResolverUrl(std::string string);

private:
};


#endif //DNS_REVERSE_PROXY_CONFIG_H
