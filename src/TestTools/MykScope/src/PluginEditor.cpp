/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MykScopeEditor::MykScopeEditor (MykScopeProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), 
    miniPianoKbd{kbdState, juce::MidiKeyboardComponent::horizontalKeyboard} , gain{1.0f}

{    
    // listen to the mini piano
    kbdState.addListener(this);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    addAndMakeVisible(pauseToggle);
    pauseToggle.addListener(this);
    
    addAndMakeVisible(gainKnob);
    gainKnob.addListener(this);
    gainKnob.setRange(0, 10);
    gainKnob.setValue(gain);
    gainKnob.setTextValueSuffix("x");
    gainKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);

    addAndMakeVisible(timeScope);
    addAndMakeVisible(freqScope);
    

    //addAndMakeVisible(miniPianoKbd);
    audioProcessor.setScopeUpdatesPerSecond(10);

    startTimer(1000 / 10);
}

MykScopeEditor::~MykScopeEditor()
{
    stopTimer();
}

void MykScopeEditor::timerCallback()
{
    timeScope.updateBuffer(audioProcessor.getScopeBuffer(), gain);
    freqScope.updateBuffer(audioProcessor.getScopeBuffer(), gain);
    repaint();
}

//==============================================================================
void MykScopeEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

   // g.setColour (juce::Colours::white);
   // g.setFont (15.0f);
   // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MykScopeEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int rowHeight = getHeight()/4; 
    int colWidth = getWidth() / 4; 
    pauseToggle.setBounds(0, 0, colWidth, rowHeight);
    gainKnob.setBounds(colWidth, 0, colWidth, rowHeight);

    freqScope.setBounds(0, rowHeight, getWidth(), rowHeight);
    timeScope.setBounds(0, rowHeight*2, getWidth(), rowHeight);
    
    //miniPianoKbd.setBounds(0, rowHeight * 5, getWidth(), rowHeight);

    
}

 void MykScopeEditor::sliderValueChanged (Slider *slider)
{
    if (slider == &gainKnob){
        gain = gainKnob.getValue();
    }
}

void MykScopeEditor::buttonClicked(Button* btn)
{
    if (btn == &pauseToggle){
       
    }
}

void MykScopeEditor::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    juce::MidiMessage msg1 = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    
}

void MykScopeEditor::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    juce::MidiMessage msg2 = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
}


