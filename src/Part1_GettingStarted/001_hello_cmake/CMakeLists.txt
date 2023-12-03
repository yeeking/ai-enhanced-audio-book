cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(hello-cmake)

add_executable(hello-cmake src/main.cpp)
set_property(TARGET hello-cmake PROPERTY CXX_STANDARD 14)
