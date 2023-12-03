cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(tiny-wav-test)

add_executable(tiny-wav-test main.cpp
                             tinywav.c
                             myk_tiny.cpp)

set_property(TARGET tiny-wav-test PROPERTY CXX_STANDARD 17)
