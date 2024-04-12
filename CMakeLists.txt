cmake_minimum_required(VERSION 3.27)
project(dns_reverse_proxy)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp" "src/config/*.cpp" "src/inet/socket/*.cpp" "src/inet/dns/*.cpp")

add_executable(dns_reverse_proxy ${SOURCES})

# LDNS Library
# libldns-dev
find_library(LDNS_LIBRARY NAMES ldns HINTS /usr/lib /usr/local/lib)
find_path(LDNS_INCLUDE_DIR ldns/ldns.h HINTS /usr/include /usr/local/include)
if (NOT LDNS_LIBRARY OR NOT LDNS_INCLUDE_DIR)
    message(FATAL_ERROR "ldns library or headers not found. install libldns-dev")
endif ()

find_package(CURL REQUIRED)

target_link_libraries(dns_reverse_proxy PRIVATE ${LDNS_LIBRARY})
target_link_libraries(dns_reverse_proxy PRIVATE CURL::libcurl)
