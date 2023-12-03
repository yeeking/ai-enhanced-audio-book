/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestPluginAudioProcessorEditor::TestPluginAudioProcessorEditor (TestPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    addAndMakeVisible(freqControl);
    addAndMakeVisible(modIndexControl);
    addAndMakeVisible(modDepthControl);
    
    freqControl.setRange(100, 1000);
    modIndexControl.setRange(0.25, 10, 0.25);
    modDepthControl.setRange(0, 1000);
    
    freqControl.addListener(this);
    modIndexControl.addListener(this);
    modDepthControl.addListener(this);

}

TestPluginAudioProcessorEditor::~TestPluginAudioProcessorEditor()
{
}

//==============================================================================
void TestPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void TestPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    freqControl.setBounds(0, 0, getWidth(), getHeight()/5);
    modIndexControl.setBounds(0, getHeight()/5, getWidth(), getHeight()/5);
    modDepthControl.setBounds(0, getHeight()/5 * 2, getWidth(), getHeight()/5);

}

 void TestPluginAudioProcessorEditor::sliderValueChanged (Slider *slider)
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
