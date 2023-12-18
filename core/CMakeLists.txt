cmake_minimum_required(VERSION 3.12)
project(AutomationCore)
set(CMAKE_CXX_STANDARD 17)

##################################################################################################
# Catch2

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(automation_test test.cpp automation_core.cpp automation_core.hpp)
target_link_libraries(automation_test PRIVATE Catch2::Catch2WithMain)