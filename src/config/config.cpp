#include "config.h"

#include <utility>

static std::string httpResolverUrl;
static bool debugMode = false;

std::string Config::getHttpResolverUrl() {
    return httpResolverUrl;
}

void Config::setHttpResolverUrl(std::string string) {
    httpResolverUrl = std::move(string);
}

void Config::setDebugMode(bool value) {
    debugMode = value;
}

bool Config::isDebugMode() {
    return &isDebugMode;
}