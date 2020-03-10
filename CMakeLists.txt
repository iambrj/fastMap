cmake_minimum_required(VERSION 3.13)
project(fastMap)

SET(GCC_COVERAGE_LINK_FLAGS    "-lpthread")
add_definitions(${GCC_COVERAGE_LINK_FLAGS})
set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(runner sub2/codes/ctrie.cpp sub2/codes/kvStore.cpp sub2/codes/ctrie.hpp)
