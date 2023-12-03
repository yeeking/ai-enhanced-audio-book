/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FMPluginEditor::FMPluginEditor (FMPluginProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    miniPianoKbd{kbdState, juce::MidiKeyboardComponent::horizontalKeyboard} 
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    addAndMakeVisible(freqControl);
    addAndMakeVisible(modIndexControl);
    addAndMakeVisible(modDepthControl);
    addAndMakeVisible(miniPianoKbd);

    addAndMakeVisible(envToggle);

    freqControl.setRange(100, 1000);
    modIndexControl.setRange(0.25, 10, 0.25);
    modDepthControl.setRange(0, 1000);
    
    freqControl.addListener(this);
    modIndexControl.addListener(this);
    modDepthControl.addListener(this);

    kbdState.addListener(this);

    envToggle.addListener(this);

}

FMPluginEditor::~FMPluginEditor()
{
}

//==============================================================================
void FMPluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void FMPluginEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    float rowHeight = getHeight()/5; 
    freqControl.setBounds(0, 0, getWidth(), rowHeight);
    modIndexControl.setBounds(0, rowHeight, getWidth(), rowHeight);
    modDepthControl.setBounds(0, rowHeight * 2, getWidth(), rowHeight);
    envToggle.setBounds(0, rowHeight * 3, getWidth(), rowHeight);
    miniPianoKbd.setBounds(0, rowHeight * 4, getWidth(), rowHeight);

}


void FMPluginEditor::buttonClicked(Button* btn)
{
    if (btn == &envToggle){
        audioProcessor.setDroneMode(envToggle.getToggleState());
    }
}



 void FMPluginEditor::sliderValueChanged (Slider *slider)
{
    if (slider == &freqControl){
        // get the slider value and do something
        DBG("Slider value " << slider->getValue());
        audioProcessor.updateFrequency(slider->getValue());
    }
    if (slider == &modIndexControl){
        // get the slider value and do something
        DBG("Mod index slider value " << slider->getValue());
        audioProcessor.setModIndex(slider->getValue());
    }
    if (slider == &modDepthControl){
        // get the slider value and do something
        DBG("Mod depth value " << slider->getValue());
        audioProcessor.setModDepth(slider->getValue());
    }
}


void FMPluginEditor::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float 
velocity)
{
    juce::MidiMessage msg1 = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    audioProcessor.addMidi(msg1, 0);
    
}

void FMPluginEditor::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float
 velocity)
{
    juce::MidiMessage msg2 = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
    audioProcessor.addMidi(msg2, 0); 
}

