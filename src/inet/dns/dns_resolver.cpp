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
    url += "?qname=" + qname;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

    std::string response;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    long httpStatusCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

    curl_easy_cleanup(curl);

    if (res == CURLE_OK && httpStatusCode < 400 && !response.empty()) {
        *ip = response;
        return 0;
    } else {
        return -1;
    }
}