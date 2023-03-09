cmake_minimum_required(VERSION 3.0)
project(boost_004_server)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(001_sync_tcp_server 001_sync_tcp_server.cpp)
add_executable(001-2_sync_tcp_server_mt 001-2_sync_tcp_server_mt.cpp)
add_executable(002_async_tcp_server 002_async_tcp_server.cpp)