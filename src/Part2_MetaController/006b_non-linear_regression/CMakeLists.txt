cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(non-linear_regression)

# update to point at your libtorch install
set(CMAKE_PREFIX_PATH "../../src_resources/libtorch/libtorch")

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(linear_regressor src/main_linear.cpp)
target_link_libraries(linear_regressor "${TORCH_LIBRARIES}")
set_property(TARGET linear_regressor PROPERTY CXX_STANDARD 14)

add_executable(non-linear_regressor src/main_nonlinear.cpp)
target_link_libraries(non-linear_regressor "${TORCH_LIBRARIES}")
set_property(TARGET non-linear_regressor PROPERTY CXX_STANDARD 14)


# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
  set_property(TARGET non-linear_regression 
               PROPERTY
               MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET non-linear_regression
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:non-linear_regression>)
endif (MSVC)
