cmake_minimum_required(VERSION 3.15)
project(media_scanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)

# ── Production sources (everything except main.cpp) ───────────────────────────
set(LIB_SOURCES
    src/scanner.cpp
    src/json_builder.cpp
    src/file_writer.cpp
    src/http_server.cpp
)

# ── Static library from production sources (reused by tests) ─────────────────
add_library(media_scanner_lib STATIC ${LIB_SOURCES})

target_include_directories(media_scanner_lib
    PUBLIC  include
    PRIVATE src
)

target_link_libraries(media_scanner_lib PUBLIC pthread)

# ── Main executable ───────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE media_scanner_lib)

# ── Tests ─────────────────────────────────────────────────────────────────────
enable_testing()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    add_subdirectory(tests)
endif()
