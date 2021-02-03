cmake_minimum_required(VERSION 3.13)
project("butler_client")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_BUILD_TYPE release)

include_directories(include)

add_library(http SHARED include/simpleHttp.cpp include/util.cpp include/util.hpp)
add_library(socket SHARED include/simpleSocket.cpp)

add_subdirectory(src/client)