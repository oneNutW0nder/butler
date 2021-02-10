include_directories(../../../include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(parser parser.cpp)

target_link_libraries(parser util httpparser)
