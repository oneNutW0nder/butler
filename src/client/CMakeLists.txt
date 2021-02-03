include_directories(../../include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(client client.cpp)

target_link_libraries(client ssl crypto fmt http socket util)