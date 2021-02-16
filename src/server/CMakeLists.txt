include_directories(../../include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(server server.cpp ../../include/simpleServer.hpp)

target_link_libraries(server argparse ssl crypto fmt)
