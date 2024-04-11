cmake_minimum_required(VERSION 3.27)
project(dns_reverse_proxy)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)

file(GLOB SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp" "src/config/*.cpp" "src/inet/socket/*.cpp" "src/inet/dns/*.cpp")

add_executable(dns_reverse_proxy ${SOURCES})

target_link_libraries(dns_reverse_proxy PRIVATE CURL::libcurl)