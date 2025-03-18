
cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(JUCE_PLUGIN_WITH_TORCH VERSION 0.0.1)

### some libtorch stuff
# https://forum.juce.com/t/using-libtorch-with-juce/41154/4
# list(APPEND CMAKE_PREFIX_PATH "<my_path_to>/libtorch-win-shared-with-deps-debug-1.12.0+cpu/libtorch")

## commands for static build against libtorch
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Ensure position-independent code
set(Torch_DIR "C:/Users/matthewyk/src/sw/libtorch-260/share/cmake/Torch")
set(TORCH_STATIC ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded") # Force static runtime if needed
set(TORCH_LIBRARIES
    torch_cpu
    c10
    pthreadpool
    cpuinfo
    XNNPACK
)
###

#set(CMAKE_PREFIX_PATH "~/src/sw/libtorch-250/") # location of libtorch
#set(CMAKE_PREFIX_PATH "C:/Users/matthewyk/src/sw/libtorch/")

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
### end libtorch stuff


add_subdirectory(../../JUCE ./JUCE)                    # If you've put JUCE in a subdirectory called JUCE

juce_add_plugin(JucePluginWithTorch
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME Yeeking                          # Specify the name of the plugin's author
    # IS_SYNTH TRUE/FALSE                       # Is this a synth or an effect?
    # NEEDS_MIDI_INPUT TRUE/FALSE               # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE YeeK               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Dem0                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "Juce Plugin With Torch")        # The name of the final executable, which can differ from the target name


juce_generate_juce_header(JucePluginWithTorch)

target_sources(JucePluginWithTorch
    PRIVATE
        src/PluginEditor.cpp
        src/PluginProcessor.cpp)


target_compile_definitions(JucePluginWithTorch
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries(JucePluginWithTorch
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
        # add torch to the libraries to link against
        "${TORCH_LIBRARIES}"
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)


# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
#  set_property(TARGET JucePluginWithTorch_Standalone
#
#              PROPERTY
#               MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET JucePluginWithTorch_Standalone
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:JucePluginWithTorch_Standalone>)
endif (MSVC)
