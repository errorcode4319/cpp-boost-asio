cmake_minimum_required(VERSION 3.0)
project(boost_basic)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()
add_executable(endpoint endpoint.cpp)
add_executable(socket socket.cpp)
add_executable(acceptor acceptor.cpp)