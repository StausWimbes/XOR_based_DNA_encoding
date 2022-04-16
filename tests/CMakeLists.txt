include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
# see: https://google.github.io/googletest/quickstart-cmake.html
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(runTests nucleotidesTest.cpp)

target_link_libraries(runTests XOR_based_DNA_encoding)
target_link_libraries(runTests gtest gtest_main)
