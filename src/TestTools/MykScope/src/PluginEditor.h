/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TimeScopeComponent.h"
#include "FreqScopeComponent.h"

//==============================================================================
/**
*/
class MykScopeEditor  :   public juce::AudioProcessorEditor,
                          public juce::Timer, 
                          // listen to buttons
                          public Button::Listener, 
                          // listen to sliders
                          public Slider::Listener, 
                          // listen to piano keyboard widget
                          private juce::MidiKeyboardState::Listener
                

{
public:
    MykScopeEditor (MykScopeProcessor&);
    ~MykScopeEditor() override;

    void timerCallback() override; 


    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged (Slider *slider) override;
    void buttonClicked(Button* btn) override;
    // from MidiKeyboardState
    void handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float
 velocity) override; 
     // from MidiKeyboardState
    void handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override; 

private:
    juce::ToggleButton pauseToggle; 
    juce::Slider gainKnob;
    TimeScopeComponent timeScope;
    FreqScopeComponent freqScope;
    
    float gain;

    // needed for the mini piano keyboard
    juce::MidiKeyboardState kbdState;
    juce::MidiKeyboardComponent miniPianoKbd; 

    

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MykScopeProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MykScopeEditor)
};
