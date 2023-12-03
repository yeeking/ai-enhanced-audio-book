cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(verb_app)

# update to point at your libtorch install
set(CMAKE_PREFIX_PATH "~/src/sw/pytorch/torch/")
# set(CMAKE_PREFIX_PATH "~/src/sw/pytorch/")
find_package(Torch REQUIRED)

add_executable(verb-app-all src/main.cpp 
                        src/tinywav/tinywav.c)

add_executable(verb-app-block src/main_block.cpp 
                        src/tinywav/tinywav.c)
                        

target_link_libraries(verb-app-all "${TORCH_LIBRARIES}")
set_property(TARGET verb-app-all PROPERTY CXX_STANDARD 17)

target_link_libraries(verb-app-block "${TORCH_LIBRARIES}")
set_property(TARGET verb-app-block PROPERTY CXX_STANDARD 17)

