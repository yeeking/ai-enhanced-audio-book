cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

project(RTNeuralLSTM VERSION 1.0.0)

add_subdirectory(../RTNeural ${CMAKE_CURRENT_BINARY_DIR}/RTNeural)
include_directories(../RTNeural)

add_executable(rtneural-lstm ./src/main.cpp)

target_link_libraries(rtneural-lstm PUBLIC RTNeural)
