cmake_minimum_required(VERSION 3.0)
project(des)

set(CMAKE_CXX_STANDARD 11)

add_executable(des main.cpp Block.hpp PermutedChoice.hpp)
