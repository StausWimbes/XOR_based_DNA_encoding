cmake_minimum_required(VERSION 3.21)
project(XOR_based_DNA_encoding)

set(CMAKE_CXX_STANDARD 23)

add_library(XOR_based_DNA_encoding SHARED encode.cpp strands.cpp strands.h)
add_executable(XOR_based_DNA_encoding_main main.cpp encode.cpp encode.h strands.cpp strands.h)