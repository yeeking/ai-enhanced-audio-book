cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(midi-markov-v1 VERSION 0.0.1)


# set up the markov library as a separate part of the build
add_library(markov-lib ../MarkovModelCPP/src/MarkovManager.cpp 
                        ../MarkovModelCPP/src/MarkovChain.cpp)

# add a new target for quickly experimenting with the Markov 
add_executable(markov-expts src/MarkovExperiments.cpp)
# # link the markov lib to the experiments executable
target_link_libraries(markov-expts  markov-lib)

add_subdirectory(../../JUCE ./JUCE)                    # If you've put JUCE in a subdirectory called JUCE

juce_add_plugin(midi-markov-v1
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME Yee-King                          # Specify the name of the plugin's author
    IS_SYNTH TRUE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT TRUE               # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT TRUE              # Does the plugin need midi output?
    IS_MIDI_EFFECT TRUE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Yeek               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE mmi0                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "midi-markov-v1")        # The name of the final executable, which can differ from the target name

# you can do this since we are not using torch any more
juce_generate_juce_header(midi-markov-v1)


target_sources(midi-markov-v1
    PRIVATE
    src/PluginEditor.cpp
    src/PluginProcessor.cpp
    ../MarkovModelCPP/src/MarkovManager.cpp
    ../MarkovModelCPP/src/MarkovChain.cpp
    src/ChordDetector.cpp
   )


target_compile_definitions(midi-markov-v1
    PUBLIC # 
        JUCE_ALSA=1
        JUCE_DIRECTSOUND=1
        JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
        #JUCE_PLUGINHOST_LADSPA=1
        #JUCE_PLUGINHOST_LV2=1
        #JUCE_PLUGINHOST_VST3=1
        JUCE_USE_OGGVORBIS=1
        #JUCE_VST3_HOST_CROSS_PLATFORM_UID=1
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(midi-markov-v1
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)


