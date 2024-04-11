#include "dns_resolver.hpp"
#include "../../config/config.h"
#include <curl/curl.h>
#include <string>

size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *response) {
    size_t totalSize = size * nmemb;
    response->append((char *) contents, totalSize);
    return totalSize;
}

int HttpDNSResolver::resolve(std::string qname, std::string *ip) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return 0;
    }

    // Construct the URL with qname parameter
    std::string url = Config::getHttpResolverUrl();
    url += qname;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

    std::string response;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK && !response.empty()) {
        *ip = response;
        return 1;
    } else {
        return 0;
    }
}