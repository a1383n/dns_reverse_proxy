#include "config.h"

#include <utility>

static std::string httpResolverUrl;

std::string Config::getHttpResolverUrl() {
    return httpResolverUrl;
}

void Config::setHttpResolverUrl(std::string string) {
    httpResolverUrl = std::move(string);
}
