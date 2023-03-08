cmake_minimum_required(VERSION 3.0)
project(boost_003_client)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(001_sync_tcp_client 001_sync_tcp_client.cpp)
add_executable(002_sync_udp_client 002_sync_udp_client.cpp)
add_executable(003_async_tcp_client 003_async_tcp_client.cpp)