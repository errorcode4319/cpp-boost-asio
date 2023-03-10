cmake_minimum_required(VERSION 3.0)
project(boost_005_http)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

message("${PROJECT_SOURCE_DIR}")

file(GLOB SOURCE_FILES *.cpp *.hpp)

add_executable(boost_http ${SOURCE_FILES})