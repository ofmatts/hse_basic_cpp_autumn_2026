cmake_minimum_required(VERSION 3.14)
project(hw07_bigint CXX)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(bigint_tests bigint.cpp test.cpp)
target_link_libraries(bigint_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(bigint_tests)
