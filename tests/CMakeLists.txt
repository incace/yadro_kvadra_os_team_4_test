cmake_minimum_required(VERSION 3.15)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST          OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(media_scanner_tests ${TEST_SOURCES})

target_include_directories(media_scanner_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(media_scanner_tests
    PRIVATE
        media_scanner_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(media_scanner_tests)
