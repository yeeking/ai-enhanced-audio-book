/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 */
class FMPluginEditor : public juce::AudioProcessorEditor,
                                       public Slider::Listener, 
                                       public Button::Listener, 
                                       private juce::MidiKeyboardState::Listener 

{
public:
  FMPluginEditor(FMPluginProcessor &);
  ~FMPluginEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

  void sliderValueChanged(Slider *slider) override;

  void buttonClicked(Button* btn) override;
  // from MidiKeyboardState
  void handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override; 
  // from MidiKeyboardState
  void handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override; 

private:
  juce::ToggleButton envToggle;

  juce::Slider freqControl;
  juce::Slider modIndexControl;
  juce::Slider modDepthControl;

  // needed for the mini piano keyboard
  juce::MidiKeyboardState kbdState;
  juce::MidiKeyboardComponent miniPianoKbd;

  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  FMPluginProcessor &audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FMPluginEditor)
};
