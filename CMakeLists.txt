cmake_minimum_required(VERSION 3.27)
project(dns_reverse_proxy)

set(CMAKE_CXX_STANDARD 17)

# Find source files
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR} "src/*.cpp" "src/config/*.cpp" "src/inet/socket/*.cpp" "src/inet/dns/*.cpp")

# Add executable
add_executable(dns_reverse_proxy ${SOURCES})

# Find Packages
find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED) # libcurl4-openssl-dev

find_library(LDNS_LIBRARY NAMES ldns HINTS /usr/lib /usr/local/lib)
find_path(LDNS_INCLUDE_DIR ldns/ldns.h HINTS /usr/include /usr/local/include)
if (NOT LDNS_LIBRARY OR NOT LDNS_INCLUDE_DIR)
    message(FATAL_ERROR "ldns library or headers not found. install libldns-dev")
endif ()

pkg_check_modules(JSONCPP REQUIRED jsoncpp) # libjsoncpp-dev
pkg_check_modules(gflags REQUIRED gflags) # libgflags-dev

# Link libraries
target_link_libraries(dns_reverse_proxy PRIVATE ${LDNS_LIBRARY})
target_link_libraries(dns_reverse_proxy PRIVATE CURL::libcurl)
target_link_libraries(dns_reverse_proxy PRIVATE ${JSONCPP_LIBRARIES})
target_link_libraries(dns_reverse_proxy PRIVATE gflags)