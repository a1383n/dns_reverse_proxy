cmake_minimum_required(VERSION 3.27)
project(dns_reverse_proxy)

set(CMAKE_CXX_STANDARD 17)
file(GLOB SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c" "src/*.cpp")

add_executable(dns_reverse_proxy ${SOURCES})
