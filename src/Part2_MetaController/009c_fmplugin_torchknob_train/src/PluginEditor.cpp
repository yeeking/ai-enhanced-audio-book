/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <torch/torch.h>


//==============================================================================
FMPluginEditor::FMPluginEditor (FMPluginProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), 
    miniPianoKbd{kbdState, juce::MidiKeyboardComponent::horizontalKeyboard} 

{    

    // listen to the mini piano
    kbdState.addListener(this);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 600);

    addAndMakeVisible(envToggle);
    envToggle.addListener(this);

    addAndMakeVisible(modIndexSlider);
   // modIndexSlider.setSliderStyle (juce::Slider::ThreeValueHorizontal);
    modIndexSlider.setRange(0.1, 10, 0.1);
    modIndexSlider.setMinAndMaxValues(0.1, 10);
    
    modIndexSlider.addListener(this);


    addAndMakeVisible(modDepthSlider);
    modDepthSlider.setRange(0, 5);
    modDepthSlider.setMinAndMaxValues(0, 5);
    modDepthSlider.addListener(this);
  //  modDepthSlider.setSliderStyle (juce::Slider::ThreeValueHorizontal);

    addAndMakeVisible(superKnob);
    superKnob.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    superKnob.setRange(0, 1);
    superKnob.addListener(this);

    addAndMakeVisible(superKnobTrain);
    superKnobTrain.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    superKnobTrain.setRange(0, 1);
    superKnobTrain.addListener(this);
    superKnobTrain.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::aliceblue);

    addAndMakeVisible(trainButton);
    trainButton.addListener(this);
    trainButton.setButtonText("train");
    addAndMakeVisible(addExampleButton);
    addExampleButton.addListener(this);
    addExampleButton.setButtonText("add");
    
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
    float rowHeight = getHeight()/6; 
    float colWidth = getWidth() / 3;
    float row = 0;
    float col = 0; 
    
    superKnobTrain.setBounds(colWidth*col, rowHeight*row, colWidth, rowHeight*2);
    col ++; 
    addExampleButton.setBounds(colWidth*col, rowHeight*row, colWidth, rowHeight);
    row ++;
    trainButton.setBounds(colWidth*col, rowHeight*row, colWidth, rowHeight);
    col ++; 
    row --; 
    superKnob.setBounds(colWidth*col, rowHeight*row, colWidth, rowHeight*2);
    row += 2;
    modIndexSlider.setBounds(0, rowHeight*row , getWidth(), rowHeight);
    row ++;
    modDepthSlider.setBounds(0, rowHeight*row, getWidth(), rowHeight);
    row ++; 
    miniPianoKbd.setBounds(0, rowHeight*row, getWidth(), rowHeight);
    row ++;
    envToggle.setBounds(0, rowHeight*row, colWidth, rowHeight);
    
}

 void FMPluginEditor::sliderValueChanged (juce::Slider *slider)
{
    if (slider == &modDepthSlider || 
        slider == &modIndexSlider){
        audioProcessor.updateFMParams(modIndexSlider.getValue(), 
                                      modDepthSlider.getValue());
    }
    if (slider == &superKnob){
        // set values of other sliders using the NN
        std::vector<float> nn_outs = nn.forward(std::vector<float>{(float)superKnob.getValue()});  
        modDepthSlider.setValue(nn_outs[0] * modDepthSlider.getMaximum());
        modIndexSlider.setValue(nn_outs[1] * modIndexSlider.getMaximum());        
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
    if (btn == &addExampleButton){
        float in = (float) superKnobTrain.getValue();
        float out1 = (float)modDepthSlider.getValue() /
                        modDepthSlider.getMaximum();
        float out2 = (float)modIndexSlider.getValue() / 
                        modIndexSlider.getMaximum();               
        nn.addTrainingData({in},{out1, out2});
    }
    if (btn ==&trainButton){
        nn.runTraining(1000000);
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


