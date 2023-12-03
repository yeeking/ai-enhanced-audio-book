cmake_minimum_required(VERSION 3.15)

project(fm-torchknob VERSION 0.0.1)

add_subdirectory(../JUCE ./JUCE)                    # If you've put JUCE in a subdirectory called JUCE

juce_add_plugin(fm-torchknob
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME Yee-King                          # Specify the name of the plugin's author
    IS_SYNTH TRUE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT TRUE               # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    NEEDS_MIDI_OUTPUT TRUE
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Yeek               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE cbc1                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "fm-torchknob")        # The name of the final executable, which can differ from the target name

juce_generate_juce_header(fm-torchknob)

target_sources(fm-torchknob
    PRIVATE
    src/PluginEditor.cpp
    src/PluginProcessor.cpp
    src/NeuralNetwork.h)


target_compile_definitions(fm-torchknob
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

    # PUBLIC # 
    #     JUCE_ALSA=1
    #     JUCE_DIRECTSOUND=1
    #     JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
    #     #JUCE_PLUGINHOST_LADSPA=1
    #     #JUCE_PLUGINHOST_LV2=1
    #     #JUCE_PLUGINHOST_VST3=1
    #     JUCE_USE_OGGVORBIS=1
    #     #JUCE_VST3_HOST_CROSS_PLATFORM_UID=1
    #     # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    #     JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    #     JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    #     JUCE_VST3_CAN_REPLACE_VST2=0)


set(CMAKE_PREFIX_PATH "../../../../sw/libtorch/") # location of libtorch
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

target_link_libraries(fm-torchknob
    PRIVATE
        juce::juce_audio_utils
        "${TORCH_LIBRARIES}"
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)


# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
set_property(TARGET fm-torchknob 
PROPERTY
MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET fm-torchknob
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:fm-torchknob>)
endif (MSVC)