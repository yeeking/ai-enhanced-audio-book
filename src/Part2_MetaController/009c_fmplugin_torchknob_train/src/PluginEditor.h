/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

//#include <JuceHeader.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "PluginProcessor.h"
#include "NeuralNetwork.h"

//==============================================================================
/**
*/
class FMPluginEditor  :   public juce::AudioProcessorEditor,
                          // listen to buttons
                          public juce::Button::Listener, 
                          // listen to sliders
                          public juce::Slider::Listener, 
                          // listen to piano keyboard widget
                          private juce::MidiKeyboardState::Listener

{
public:
    FMPluginEditor (FMPluginProcessor&);
    ~FMPluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged (juce::Slider *slider) override;
    void buttonClicked(juce::Button* btn) override;
    // from MidiKeyboardState
    void handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float
 velocity) override; 
     // from MidiKeyboardState
    void handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override; 


private:
    juce::ToggleButton envToggle; 
    juce::Slider modIndexSlider;
    juce::Slider modDepthSlider;
    juce::Slider superKnob; 
    juce::Slider superKnobTrain; 

    juce::TextButton addExampleButton;
    juce::TextButton trainButton;
    

    // needed for the mini piano keyboard
    juce::MidiKeyboardState kbdState;
    juce::MidiKeyboardComponent miniPianoKbd; 

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FMPluginProcessor& audioProcessor;

    NeuralNetwork nn{1, 2}; 
      
    // torch::nn::Linear linear{nullptr};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FMPluginEditor)
};
