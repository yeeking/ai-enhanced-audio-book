cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(lstm-performance)

# update to point at your libtorch install

#set(CMAKE_PREFIX_PATH "C:/Users/matthewyk/src/sw/libtorch/")
set(CMAKE_PREFIX_PATH "~/src/sw/libtorch-2/pytorch-install/")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(lstm-performance src/main.cpp
                                 ../tinywav/tinywav.c
                                 ../tinywav/myk_tiny.cpp)

set_property(TARGET lstm-performance PROPERTY CXX_STANDARD 17)

target_link_libraries(lstm-performance "${TORCH_LIBRARIES}")

# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
  set_property(TARGET lstm-performance 
               PROPERTY
               MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET lstm-performance
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:lstm-performance>)
endif (MSVC)
