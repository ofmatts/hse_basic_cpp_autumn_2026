cmake_minimum_required(VERSION 3.14)
project(hw04_mygrep CXX)

# std::filesystem появился только в 17-м, в дз 2/3 хватало 14-го
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(mygrep_tests mygrep.cpp test.cpp)
target_link_libraries(mygrep_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(mygrep_tests)

# сама утилита
add_executable(mygrep main.cpp mygrep.cpp)
