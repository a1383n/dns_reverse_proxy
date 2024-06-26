#ifndef DNS_REVERSE_PROXY_UTILITY_H
#define DNS_REVERSE_PROXY_UTILITY_H

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base *>(ptr) != nullptr;
}

#endif //DNS_REVERSE_PROXY_UTILITY_H
