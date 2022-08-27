cmake_minimum_required(VERSION 3.21)
project(XOR_based_DNA_encoding)

set(CMAKE_CXX_STANDARD 23)

# Windows shared library fix
# see: https://cmake.org/cmake/help/latest/guide/tutorial/Selecting%20Static%20or%20Shared%20Libraries.html
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(LIB_NAME xor_code)
set(LIB_TESTS_NAME xor_code_tests)
set(LIB_EXAMPLE_NAME xor_code_run)

add_subdirectory(cpp)
