cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

project(RTNeuralLSTM VERSION 1.0.0)

set(CMAKE_PREFIX_PATH "~/src/sw/libtorch-2/")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")


add_subdirectory(../RTNeural ${CMAKE_CURRENT_BINARY_DIR}/RTNeural)
include_directories(../RTNeural)

add_executable(rtneural-lstm ./src/main_rtneural_basic.cpp)
add_executable(rtneural-vs-script ./src/main_torchscript_vs_rtneural.cpp)
add_executable(performance ./src/performance_test.cpp)

target_link_libraries(rtneural-lstm PUBLIC RTNeural)
target_link_libraries(rtneural-vs-script  PUBLIC RTNeural "${TORCH_LIBRARIES}")
target_link_libraries(performance  PUBLIC RTNeural "${TORCH_LIBRARIES}")