/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
//#include "PluginEditor.h"
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
                       ), 
    // initialiser list
    baseFreq{400.0f}, 
    carrierDPhase{0.0f}, 
    modDPhase{0.0f},
    two_pi{3.1415927f * 2}

#endif
{
    DBG("YeekFMBasic cons");
    phases[0] = 0;
    phases[1] = 0;
    phases[2] = 0;
    phases[3] = 0;

    // set up the params
    addParameter (modDepthP = 
        new juce::AudioParameterFloat (
            "modDepth", // parameterID
            "Mod Depth", // parameter name
            0.0f,   // minimum value
            1.0f,   // maximum value
            1.0f)); // default value
    addParameter (modIndexP = 
        new juce::AudioParameterFloat (
            "modIndex", // parameterID
            "Mod Index", // parameter name
            0.1f,   // minimum value
            5.0f,   // maximum value
            0.25f)); // default value          
   
}

YeekSineAudioProcessor::~YeekSineAudioProcessor()
{
   // delete phase; 
   DBG("YeekFMBasic dest");
   
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
    DBG("YeekFMBasic getNumPrograms");
   
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int YeekSineAudioProcessor::getCurrentProgram()
{
    DBG("YeekFMBasic getCurrentProgram");

    return 0;
}

void YeekSineAudioProcessor::setCurrentProgram (int index)
{
    DBG("YeekFMBasic setCurrentProgram");

}

const juce::String YeekSineAudioProcessor::getProgramName (int index)
{
    DBG("YeekFMBasic getProgramName");

    return {};
}

void YeekSineAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    DBG("YeekFMBasic changeProgramName");

}

//==============================================================================
void YeekSineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //dphase = YeekSineAudioProcessor::getDPhase(400, sampleRate, two_pi);
    // compute dphases based on sample rate
    carrierDPhase = YeekSineAudioProcessor::getDPhase(baseFreq , sampleRate, two_pi);
    modDPhase = YeekSineAudioProcessor::getDPhase(
                    baseFreq * modIndexP->get(), 
                    getSampleRate(), 
                    two_pi);
    DBG("YeekFMBasic:: prepare to play");
}

void YeekSineAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    DBG("YeekFMBasic releaseResources");

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool YeekSineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    DBG("YeekFMBasic isBusesLayoutSupported");

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


    juce::ScopedNoDenormals noDenormals;
    // float modDepth{modDepthP->get()};
    // float modIndex{modIndexP->get()};
    float modDepth = *modDepthP;
    float modIndex = *modIndexP;
    unsigned int totalNumInputChannels  = getTotalNumInputChannels();
    unsigned int totalNumOutputChannels = getTotalNumOutputChannels();
    for (unsigned int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    // scan the midi nmessages. If there is one there, update the dphase 
    // according to the note
    float modY, carrF, carrY;// amplitudes of the two oscillators
    // recompute mod dphase once per block
    // in case the modIndexP parameter changed
    modDPhase = YeekSineAudioProcessor::getDPhase(
                baseFreq * modIndex, 
                getSampleRate(), 
                two_pi);
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sind=0;sind<buffer.getNumSamples(); ++sind)
        {

            // first off, write the carrier output
            // into the output buffer
            carrY = std::sin(phases[channel]);
            channelData[sind] = carrY * 0.25;
            // now computer the phase change for the next sample
            // modulator output
            modY = std::sin(phases[channel+2]);
            // scale modulator by modDepthP
            // from -1 to 1 to -baseF * modDepth to baseF * modDepth
            modY *= modDepth * baseFreq;
            // compute new carrier freq
            carrF = baseFreq + modY; 
            // we update carrier dphase every sample 
            carrierDPhase = YeekSineAudioProcessor::getDPhase(
                carrF, getSampleRate(), two_pi
            );
            phases[channel] = fmod((phases[channel] + carrierDPhase), two_pi); 
            // change in modulator phase
            // does not change every sample, only
            // when base freq changes so no need 
            // to update it here
            phases[channel+2] = fmod((phases[channel+2] + modDPhase), two_pi);    
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
            baseFreq = (float) juce::MidiMessage::getMidiNoteInHertz(message.getNoteNumber());
            // update modDPhase - carrierDPhase
            // is updated every sample in processBlock
            modDPhase = YeekSineAudioProcessor::getDPhase(
                // mod f = baseF * modIndex
                baseFreq * modIndexP->get(), 
                getSampleRate(), 
                two_pi);
        }
 
        processedMidi.addEvent (message, time);
    }
 
    midiMessages.swapWith (processedMidi);
}

//==============================================================================
bool YeekSineAudioProcessor::hasEditor() const
{
    DBG("YeekFMBasic hasEditor");

    //return true; // (change this to false if you choose to not supply an editor)
    return true; // if this is false, the UI does not update???
}

juce::AudioProcessorEditor* YeekSineAudioProcessor::createEditor()
{
  DBG("YeekFMBasic createEditor");
  return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void YeekSineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
            DBG("YeekFMBasic getStateInformation");

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void YeekSineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
                DBG("YeekFMBasic setStateInformation");

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
                    DBG("YeekFMBasic createPluginFilter");

    return new YeekSineAudioProcessor();
}


float YeekSineAudioProcessor::getDPhase(float freq, float sampleRate, float two_pi){
    float dphase = two_pi / sampleRate * freq;
    return dphase; 
}
