/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
YeekSineAudioProcessor::YeekSineAudioProcessor()
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
{
   DBG("YeekSineFreq cons");
   phase[0] = 0;
   phase[1] = 0;
}

YeekSineAudioProcessor::~YeekSineAudioProcessor()
{
   // delete phase; 
   DBG("YeekSineFreq dest");
   
}

//==============================================================================
const juce::String YeekSineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool YeekSineAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool YeekSineAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool YeekSineAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double YeekSineAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int YeekSineAudioProcessor::getNumPrograms()
{
    DBG("YeekSineFreq getNumPrograms");
   
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int YeekSineAudioProcessor::getCurrentProgram()
{
    DBG("YeekSineFreq getCurrentProgram");

    return 0;
}

void YeekSineAudioProcessor::setCurrentProgram (int index)
{
            DBG("YeekSineFreq setCurrentProgram");

}

const juce::String YeekSineAudioProcessor::getProgramName (int index)
{
    DBG("YeekSineFreq getProgramName");

    return {};
}

void YeekSineAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    DBG("YeekSineFreq changeProgramName");

}

//==============================================================================
void YeekSineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    dphase = YeekSineAudioProcessor::getDPhase(400, sampleRate, two_pi);
    DBG("YeekSineFreq:: prepare to play");
}

void YeekSineAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    DBG("YeekSineFreq releaseResources");

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool YeekSineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    DBG("YeekSineFreq isBusesLayoutSupported");

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

void YeekSineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
   // DBG("YeekSineFreq processBlock");

    juce::ScopedNoDenormals noDenormals;
    unsigned int totalNumInputChannels  = getTotalNumInputChannels();
    unsigned int totalNumOutputChannels = getTotalNumOutputChannels();
    for (unsigned int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    // scan the midi nmessages. If there is one there, update the dphase 
    // according to the note
    for (unsigned int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (unsigned int sind=0;sind<buffer.getNumSamples(); ++sind)
        {
            channelData[sind] = std::sin(phase[channel]) * 0.25;
            phase[channel] = fmod((phase[channel] + dphase), two_pi); 
        }
    }

    juce::MidiBuffer processedMidi;
 
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        const auto time = metadata.samplePosition;
 
        if (message.isNoteOn())
        {
            message = juce::MidiMessage::noteOn (message.getChannel(),
                                                 message.getNoteNumber(),
                                                 (juce::uint8) message.getVelocity());
            dphase = YeekSineAudioProcessor::getDPhase(
                (float) (juce::MidiMessage::getMidiNoteInHertz(message.getNoteNumber())),
                getSampleRate(), 
                two_pi);
     DBG("YeekSineFreq: procBlock changed dphase " << dphase);

        }
 
        processedMidi.addEvent (message, time);
    }
 
    midiMessages.swapWith (processedMidi);
}

//==============================================================================
bool YeekSineAudioProcessor::hasEditor() const
{
    DBG("YeekSineFreq hasEditor");

    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* YeekSineAudioProcessor::createEditor()
{
        DBG("YeekSineFreq createEditor");

    return new YeekSineAudioProcessorEditor (*this);
}

//==============================================================================
void YeekSineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
            DBG("YeekSineFreq getStateInformation");

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void YeekSineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
                DBG("YeekSineFreq setStateInformation");

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
                    DBG("YeekSineFreq createPluginFilter");

    return new YeekSineAudioProcessor();
}


float YeekSineAudioProcessor::getDPhase(float freq, float sampleRate, float two_pi){
    float dphase = two_pi / sampleRate * freq;
    return dphase; 
}
