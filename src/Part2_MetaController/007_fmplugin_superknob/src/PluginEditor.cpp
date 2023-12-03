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

    // listen to the mini piano
    kbdState.addListener(this);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 600);

    addAndMakeVisible(envToggle);
    envToggle.addListener(this);

    addAndMakeVisible(modIndexSlider);
    modIndexSlider.setSliderStyle (juce::Slider::ThreeValueHorizontal);
    modIndexSlider.setRange(0.1, 10, 0.1);
    modIndexSlider.addListener(this);

    addAndMakeVisible(modDepthSlider);
    modDepthSlider.setRange(0, 5);
    modDepthSlider.addListener(this);
    modDepthSlider.setSliderStyle (juce::Slider::ThreeValueHorizontal);

    addAndMakeVisible(superKnob);
    superKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    superKnob.setRange(0, 1);

    superKnob.addListener(this);

    addAndMakeVisible(miniPianoKbd);

}

FMPluginEditor::~FMPluginEditor()
{
}

//==============================================================================
void FMPluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

   // g.setColour (juce::Colours::white);
   // g.setFont (15.0f);
   // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void FMPluginEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    float rowHeight = getHeight()/5; 
    float colWidth = getWidth() / 3;
    float row = 0;

    envToggle.setBounds(0, rowHeight*row, colWidth, rowHeight);
    superKnob.setBounds(colWidth, rowHeight*row, colWidth*2, rowHeight*2);
    row +=2;
    modIndexSlider.setBounds(0, rowHeight*row , getWidth(), rowHeight);
    row ++;
    modDepthSlider.setBounds(0, rowHeight*row, getWidth(), rowHeight);
    row ++; 
    miniPianoKbd.setBounds(0, rowHeight*row, getWidth(), rowHeight);
    
}

 void FMPluginEditor::sliderValueChanged (juce::Slider *slider)
{
    if (slider == &modDepthSlider || 
        slider == &modIndexSlider){
        audioProcessor.updateFMParams(modIndexSlider.getValue(), 
                                      modDepthSlider.getValue());
    }
    if (slider == &superKnob){
        // set values of other sliders using the 
        // in and out markers and simple line expression
        double weight, bias;
        // superknob is in range 0-1
        // find out scalar to range of 
        // modindex slider
        weight = modIndexSlider.getMaxValue() - modIndexSlider.getMinValue();
        bias = modIndexSlider.getMinValue();
        modIndexSlider.setValue((superKnob.getValue() * weight) + bias);
        
        weight = modDepthSlider.getMaxValue() - modDepthSlider.getMinValue();
        bias = modDepthSlider.getMinValue();
        modDepthSlider.setValue((superKnob.getValue() * weight) + bias);
        
    }
}

void FMPluginEditor::buttonClicked(juce::Button* btn)
{
    if (btn == &envToggle){
        double envLen = 0;
        if (envToggle.getToggleState()) { // one
            envLen = 1;
        }
        audioProcessor.setEnvLength(envLen);
    }
    
}

void FMPluginEditor::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    juce::MidiMessage msg1 = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    audioProcessor.addMidi(msg1, 0);
    
}

void FMPluginEditor::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    juce::MidiMessage msg2 = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
    audioProcessor.addMidi(msg2, 0); 
}


