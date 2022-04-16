cmake_minimum_required(VERSION 3.21)
project(XOR_based_DNA_encoding)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES main.cpp)
add_executable(XOR_based_DNA_encoding_run ${SOURCE_FILES})

include_directories(src)
add_subdirectory(src)

target_link_libraries(XOR_based_DNA_encoding_run XOR_based_DNA_encoding)

add_subdirectory(tests)
