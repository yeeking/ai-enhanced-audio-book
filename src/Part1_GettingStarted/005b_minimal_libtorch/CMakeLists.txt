cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(minimal-libtorch)


# update to point at your libtorch install
set(CMAKE_PREFIX_PATH "../../../../sw/libtorch")
# set(CMAKE_PREFIX_PATH "~/src/sw/pytorch/")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(minimal-libtorch src/main.cpp)
target_link_libraries(minimal-libtorch "${TORCH_LIBRARIES}")
set_property(TARGET minimal-libtorch PROPERTY CXX_STANDARD 14)

# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
  set_property(TARGET minimal-libtorch 
               PROPERTY
               MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET minimal-libtorch
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:minimal-libtorch>)
endif (MSVC)
