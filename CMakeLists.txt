cmake_minimum_required(VERSION 3.19)
project("butler_client")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_BUILD_TYPE debug)

include_directories(include)

add_library(http SHARED include/simpleHttp.cpp)
add_library(socket SHARED include/simpleSocket.cpp)
add_library(util SHARED include/util.cpp)
add_library(argparse INTERFACE include/argparse.hpp)

add_subdirectory(src/client)
add_subdirectory(src/server/parser)
