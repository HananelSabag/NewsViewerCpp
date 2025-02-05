cmake_minimum_required(VERSION 3.14)
project(NewsViewer)

set(CMAKE_CXX_STANDARD 17)

# Ensure OpenSSL is found
find_package(OpenSSL REQUIRED)

# Define OpenSSL support for httplib
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR})

# Add source files
add_executable(NewsViewer main.cpp src/news_fetcher.cpp)

# Link OpenSSL and Windows Sockets
target_link_libraries(NewsViewer OpenSSL::SSL OpenSSL::Crypto ws2_32 crypt32)
