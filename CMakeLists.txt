cmake_minimum_required(VERSION 3.13)
project(fastMap)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(fastMap
        src/ctrie.cpp
        src/ctrie.hpp)
