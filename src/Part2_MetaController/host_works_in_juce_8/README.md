# VST host code adapted for JUCE 8

Its Dec 2025 and I have found that the VST code does not build or work properly with the latest version of JUCE

This version does work:

To build it: 

```
git clone https://github.com/juce-framework/JUCE.git
cmake -B build .
# set j to number of cores you want to use
cmake --build build --config Debug -j 8
```
