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

    /** load a plugin from the sent file*/
    void loadPlugin(const juce::File& pluginFile);

private:
    
    /** stores messages added from the addMidi function*/
    juce::MidiBuffer midiToProcess;
    
    juce::AudioPluginFormatManager pluginFormatManager;
    juce::KnownPluginList knownPluginList;
    juce::OwnedArray<juce::PluginDescription> pluginDescriptions;
    int vstFormatInd; 
    std::unique_ptr<juce::AudioPluginInstance> pluginInstance; 


    // the graph
    std::unique_ptr<juce::AudioProcessorGraph> audioProcGraph;
    // graph components: processors
    std::unique_ptr<juce::AudioPluginInstance>  inputProc;
    std::unique_ptr<juce::AudioPluginInstance>  outputProc;
    std::unique_ptr<juce::AudioPluginInstance>  midiInputProc;
    std::unique_ptr<juce::AudioPluginInstance>  midiOutputProc;
    // graph components: nodes that wrap the processors
    juce::AudioProcessorGraph::Node::Ptr  inputNode;
    juce::AudioProcessorGraph::Node::Ptr  outputNode;
    juce::AudioProcessorGraph::Node::Ptr  midiInNode;
    juce::AudioProcessorGraph::Node::Ptr  midiOutNode;

    // the node for the plugin
    juce::AudioProcessorGraph::Node::Ptr  pluginNode;


    void addPluginToGraph();

      //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginHostProcessor)
};
