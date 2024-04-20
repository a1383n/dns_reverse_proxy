cmake_minimum_required(VERSION 3.27)
project(dns_reverse_proxy)

set(CMAKE_CXX_STANDARD 17)

# Find source files
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR} "src/*.cpp" "src/config/*.cpp" "src/inet/socket/*.cpp" "src/inet/dns/*.cpp")

# Add executable
add_executable(dns_reverse_proxy ${SOURCES})

# Find Packages
find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
pkg_check_modules(LDNS REQUIRED ldns)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# Include directories
target_include_directories(dns_reverse_proxy PRIVATE ${LDNS_INCLUDE_DIR})
target_include_directories(dns_reverse_proxy PRIVATE ${CURL_INCLUDE_DIR})
target_include_directories(dns_reverse_proxy PRIVATE ${JSONCPP_INCLUDE_DIRS})

# Link libraries
target_link_libraries(dns_reverse_proxy PRIVATE ${LDNS_LIBRARY})
target_link_libraries(dns_reverse_proxy PRIVATE CURL::libcurl)
target_link_libraries(dns_reverse_proxy PRIVATE ${JSONCPP_LIBRARIES})
