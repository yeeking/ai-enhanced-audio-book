cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(libtorch_regressor)

# update to point at your libtorch install
set(CMAKE_PREFIX_PATH "~/src/sw/libtorch-2")

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(libtorch_regressor src/main.cpp)
target_link_libraries(libtorch_regressor "${TORCH_LIBRARIES}")
set_property(TARGET libtorch_regressor PROPERTY CXX_STANDARD 17)

# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
  set_property(TARGET libtorch_regressor 
               PROPERTY
               MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET libtorch_regressor
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:libtorch_regressor>)
endif (MSVC)
