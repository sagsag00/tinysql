cmake_minimum_required(VERSION 3.14)
project(TinySQL)

set(CMAKE_CXX_STANDARD 17)

# Fetch gtest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

# Core library
add_library(tiny_sql_lib
    src/tokenizer.cpp
    src/parser.cpp
    src/engine.cpp
    src/table.cpp
    src/printer.cpp
)
target_include_directories(tiny_sql_lib PUBLIC src/)

# Main exe
add_executable(tiny_sql src/main.cpp)
target_link_libraries(tiny_sql PRIVATE tiny_sql_lib)

# Tests
enable_testing()

add_executable(tiny_sql_tests
    tests/test_tokenizer.cpp
    tests/test_parser.cpp
    tests/test_engine.cpp
)
target_link_libraries(tiny_sql_tests PRIVATE
    tiny_sql_lib
    gtest_main
)

add_test(NAME TinySQLTests COMMAND tiny_sql_tests)