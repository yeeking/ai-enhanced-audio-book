/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class YeekSineAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    YeekSineAudioProcessor();
    ~YeekSineAudioProcessor() override;

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

private:
    /// @brief maintain one phase for each channel, each oscillator : 4
    std::unique_ptr<float[]> phases{new float[4]};
    /// @brief the f0 taken from incoming midi normally 
    float baseFreq;
    /// @brief how much does carrier phase change by each step
    float carrierDPhase; 
    /// @brief how much does modulator phase change by each step
    float modDPhase;
    float two_pi;
    
    /// @brief calculates the phase change per sample for the sent frequency and sample rate
    static float getDPhase(float freq, float sampleRate, float two_pi);

    /// @brief parameter to control mod index (ratio between carrier and mod freq)
    juce::AudioParameterFloat* modIndexP;
    /// @brief parameter to control mod depth (scalar applied to mod output prior to modulating carrier freq)
    juce::AudioParameterFloat* modDepthP;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YeekSineAudioProcessor)
};
