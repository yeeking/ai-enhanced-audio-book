cmake_minimum_required(VERSION 3.15)

project(chord-detector VERSION 0.0.1)

set (CMAKE_CXX_STANDARD 14)

# add a new target for quickly experimenting with the Markov 
add_executable(chord-detector src/ChordDetector.cpp src/ChordDetectTest.cpp)

