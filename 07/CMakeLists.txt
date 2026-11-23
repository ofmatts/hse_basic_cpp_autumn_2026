cmake_minimum_required(VERSION 3.14)
project(hw07_vector CXX)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(vector_tests test.cpp)
target_link_libraries(vector_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(vector_tests)
