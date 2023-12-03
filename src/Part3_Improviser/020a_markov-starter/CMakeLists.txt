cmake_minimum_required(VERSION 3.15)

project(markov-starter VERSION 0.0.1)

set (CMAKE_CXX_STANDARD 14)

# set up the markov library as a separate part of the build
add_library(markov-lib ../MarkovModelCPP/src/MarkovManager.cpp 
                       ../MarkovModelCPP/src/MarkovChain.cpp)

# add a new target for quickly experimenting with the Markov 
add_executable(markov-starter src/main.cpp)

# link the markov lib to the experiments executable
target_link_libraries(markov-starter  markov-lib)

