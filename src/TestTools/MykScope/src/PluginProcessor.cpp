/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MykScopeProcessor::MykScopeProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
, scopeBuffer{1, 512}, scopeBufferLength{512}, tick{0}, updatesPerSecond{2}
{
    scopeBuffer.clear();
}

MykScopeProcessor::~MykScopeProcessor()
{
}

//==============================================================================
const juce::String MykScopeProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MykScopeProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MykScopeProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MykScopeProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MykScopeProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MykScopeProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MykScopeProcessor::getCurrentProgram()
{
    return 0;
}

void MykScopeProcessor::setCurrentProgram (int index)
{
}

const juce::String MykScopeProcessor::getProgramName (int index)
{
    return {};
}

void MykScopeProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MykScopeProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

}

void MykScopeProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MykScopeProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MykScopeProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // decide if we want to update or not
    tick ++;
    unsigned long blocksPerSecond = getSampleRate() / buffer.getNumSamples();
    unsigned long updateInterval = blocksPerSecond / updatesPerSecond; 
    if (updateInterval < 1) updateInterval = 1;
    if (tick % updateInterval == 0) {
        updateScopeBuffer(buffer);
    }
}

//==============================================================================
bool MykScopeProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MykScopeProcessor::createEditor()
{
    return new MykScopeEditor (*this);
}

//==============================================================================
void MykScopeProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MykScopeProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void MykScopeProcessor::updateScopeBuffer(const juce::AudioBuffer<float>& buffer)
{
    // auto* buffRead = buffer.getReadPointer(0);
    // auto* buffWrite = scopeBuffer.getWritePointer(0);

    // // work out the bufLength which is the number of samples we will copy
    // scopeBufferLength = buffer.getNumSamples();
    // if (scopeBufferLength > scopeBuffer.getNumSamples()){
    //     scopeBufferLength = scopeBuffer.getNumSamples();
    // }

    // for (int ind = 0;ind<scopeBufferLength; ++ind){
    //     buffWrite[ind] = buffRead[ind];
    // }

    scopeBufferLength = buffer.getNumSamples();
    if (scopeBufferLength > scopeBuffer.getNumSamples()){
        scopeBufferLength = scopeBuffer.getNumSamples();
    }
    scopeBuffer.copyFrom(0, //int destChannel,
                   0, //int destStartSample,
                   buffer, //const AudioBuffer& source,
                   0, //int sourceChannel,
                   0, //int sourceStartSample,
                   scopeBufferLength);//int numSamples);


}

const juce::AudioBuffer<float>& MykScopeProcessor::getScopeBuffer()
{
    return this->scopeBuffer;
}

void MykScopeProcessor::setScopeUpdatesPerSecond(unsigned long upsPerSec)
{
    updatesPerSecond = upsPerSec; 
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MykScopeProcessor();
}




