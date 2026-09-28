cmake_minimum_required(VERSION 3.14)
project(hw02_allocator CXX)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(allocator_test allocator.cpp test.cpp)
target_link_libraries(allocator_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(allocator_test)

