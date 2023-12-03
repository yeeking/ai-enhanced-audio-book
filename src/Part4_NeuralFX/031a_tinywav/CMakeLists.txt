cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

project(tinywav VERSION 1.0.0)

add_executable(tinywav
                src/main.cpp
                ../tinywav/tinywav.c
                ../tinywav/myk_tiny.cpp)

