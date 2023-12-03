cmake_minimum_required(VERSION 3.15)

project(markovcpp VERSION 0.0.1)

set (CMAKE_CXX_STANDARD 14)

# set up the markov library as a separate part of the build
add_library(markov-lib src/MarkovManager.cpp  src/MarkovChain.cpp)

# add a new target for quickly experimenting with the Markov 
add_executable(markov-tests src/MarkovTest.cpp)

# link the markov lib to the experiments executable
target_link_libraries(markov-tests  markov-lib)

