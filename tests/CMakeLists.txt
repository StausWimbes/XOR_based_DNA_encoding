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


set(SOURCE_FILES
        nucleotidesTest.cpp
        )

add_executable(runTests ${SOURCE_FILES})


target_link_libraries(runTests xorCode)
target_link_libraries(runTests gtest gtest_main)
