cmake_minimum_required(VERSION 3.19)
project("butler_client")

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_BUILD_TYPE debug)

include_directories(include)

add_library(http SHARED src/lib/simpleHttp.cpp)
add_library(socket SHARED src/lib/simpleSocket.cpp)
add_library(util SHARED src/lib/util.cpp)
add_library(httpparser SHARED src/lib/httpParser.cpp)
add_library(simpleServer SHARED src/lib/simpleServer.cpp)
add_library(argparse INTERFACE include/argparse.hpp)

add_subdirectory(src/client)
add_subdirectory(src/server/)
add_subdirectory(src/server/parser)
