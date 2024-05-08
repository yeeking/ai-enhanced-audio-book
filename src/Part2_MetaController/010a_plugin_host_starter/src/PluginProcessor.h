/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

// #include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>


//==============================================================================
/**
*/
class PluginHostProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    PluginHostProcessor();
    ~PluginHostProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // void updateFrequency(double newFreq);
    // void updateFMParams(double _modIndex, double _modDepth);

    // /** add some midi to be played at the sent sample offset*/
    // /** set env length in seconds*/
    // void setEnvLength(double envLengthSecs);

    void addMidi(juce::MidiMessage msg, int sampleOffset);

private:
    // double phase;
    // double dphase;
    // double baseFrequency;  

    // double carrPhase;
    // double carrDPhase;

    // //double modFreq; 
    // double modPhase; 
    // double modDPhase; 
    
    // double modIndex;
    // double modDepth;

    // double amp;
    // double ampTarget;
    // double dAmp;  
    // double ampMax; 

    // double getDPhase(double freq, double sampleRate);

    /** stores messages added from the addMidi function*/
    juce::MidiBuffer midiToProcess;


      //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginHostProcessor)
};
