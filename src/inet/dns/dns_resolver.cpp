#include "dns_resolver.hpp"
#include "../../config/config.h"
#include <curl/curl.h>
#include <string>
#include <stdexcept>

size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *response) {
    size_t totalSize = size * nmemb;
    response->append((char *) contents, totalSize);
    return totalSize;
}

std::string HttpDNSResolver::resolve(std::string qname) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Fail to init curl");
    }

    std::string url = Config::getHttpResolverUrl() + qname;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

    std::string response;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK && !response.empty()) {
        return response;
    } else {
        throw std::runtime_error("Invalid response");
    }
}

std::vector<std::string> HttpDNSResolver::resolve(std::vector<std::string> qname) {
    return std::vector<std::string>();
}
