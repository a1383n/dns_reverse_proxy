#include "dns_resolver.hpp"
#include "../../config/config.h"
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <string>

size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *response) {
    size_t totalSize = size * nmemb;
    response->append((char *) contents, totalSize);
    return totalSize;
}

int HttpDNSResolver::resolve(std::string clientIp, std::string qname, std::string *ip) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return -1;
    }

    std::string url = Config::getHttpResolverUrl();

    Json::Value postData;
    postData["q"] = qname;
    postData["ip"] = clientIp;
    Json::StreamWriterBuilder writer;
    std::string postFields = Json::writeString(writer, postData);

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postFields.size());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(nullptr, "Content-Type: application/json"));

    CURLcode res = curl_easy_perform(curl);
    long httpStatusCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

    curl_easy_cleanup(curl);

    if (res == CURLE_OK && httpStatusCode < 400 && !response.empty()) {
        try {
            Json::CharReaderBuilder readerBuilder;
            Json::Value jsonResponse;
            std::string errs;

            std::istringstream responseStream(response);
            if (Json::parseFromStream(readerBuilder, responseStream, &jsonResponse, &errs)) {
                if (jsonResponse["ok"].asBool()) {
                    *ip = jsonResponse["result"]["address"].asString();
                    return 0;
                } else {
                    return -2;
                }
            } else {
                return -1;
            }
        } catch (const std::exception &e) {
            return -1;
        }
    } else {
        return -1;
    }
}
