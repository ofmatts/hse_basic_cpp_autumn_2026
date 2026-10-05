cmake_minimum_required(VERSION 3.14)
project(hw03_token_parser CXX)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(parser_tests token_parser.cpp test.cpp)
target_link_libraries(parser_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(parser_tests)

# демо из условия, читает строки со stdin и печатает токены
add_executable(parser_demo main.cpp token_parser.cpp)
