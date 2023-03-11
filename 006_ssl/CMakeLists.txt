cmake_minimum_required(VERSION 3.0)
project(boost_006_ssl)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

message("${PROJECT_SOURCE_DIR}")

file(GLOB SOURCE_FILES *.cpp *.hpp)

add_executable(asio_ssl ${SOURCE_FILES})

target_link_libraries(asio_ssl OpenSSL::SSL)