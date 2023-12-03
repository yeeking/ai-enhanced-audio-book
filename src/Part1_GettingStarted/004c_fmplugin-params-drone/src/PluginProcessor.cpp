/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FMPluginProcessor::FMPluginProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         )
#endif
      ,
      phase{0}, dphase{0}, frequency{440}, amp{0}, ampTarget{0}, dAmp{0.00005}, mod_phase{0}, mod_dphase{0}, mod_index{0.5}, mod_depth{100}, droneMode{false}
{
    // Create a parameter object
    modIndexParam = new juce::AudioParameterFloat("ModInd",    // Parameter ID
                                                  "Mod index", // Parameter name
                                                  0.0f,        // Minimum value
                                                  10.0f,       // Maximum value
                                                  0.5f);       // Default value

    // Add the parameter to the parameters array
    addParameter(modIndexParam);
    // Create a parameter object
    modDepthParam = new juce::AudioParameterFloat("ModDep",    // Parameter ID
                                                  "Mod depth", // Parameter name
                                                  0.0f,        // Minimum value
                                                  1000.0f,     // Maximum value
                                                  100.0f);     // Default value

    // Add the parameter to the parameters array
    addParameter(modDepthParam);
}

FMPluginProcessor::~FMPluginProcessor()
{
}

//==============================================================================
const juce::String FMPluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FMPluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FMPluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FMPluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FMPluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FMPluginProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int FMPluginProcessor::getCurrentProgram()
{
    return 0;
}

void FMPluginProcessor::setCurrentProgram(int index)
{
}

const juce::String FMPluginProcessor::getProgramName(int index)
{
    return {};
}

void FMPluginProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void FMPluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    dphase = getDPhase(frequency, sampleRate);
}

void FMPluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FMPluginProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void FMPluginProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{

    if (midiToProcess.getNumEvents() > 0)
    {
        midiMessages.addEvents(midiToProcess, midiToProcess.getFirstEventTime(), midiToProcess.getLastEventTime() + 1, 0);
        midiToProcess.clear();
    }

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    double mod{0.0f};

    // update frequency and therefore phase values based on params
    mod_dphase = getDPhase(frequency * static_cast<double>(*modIndexParam), getSampleRate());
    mod_depth = static_cast<double>(*modDepthParam);
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        if (channel == 0)
        {
            auto *channelData = buffer.getWritePointer(channel);
            int numSamples = buffer.getNumSamples();
            for (int sInd = 0; sInd < numSamples; ++sInd)
            {
                channelData[sInd] = (float)(std::sin(phase) * amp);
                // update the frequency of the carrier using the modulator
                mod = std::sin(mod_phase);
                mod *= mod_depth;
                dphase = getDPhase(frequency + mod, getSampleRate());

                phase += dphase;
                mod_phase += mod_dphase;
                if (amp > ampTarget)
                    amp -= dAmp;
                if (amp < ampTarget)
                    amp += dAmp;
            }
        }
    }

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        // DBG("processBlock:: Got message " << message.getDescription());
        if (message.isNoteOn())
        {
            ampTarget = 0.5;
            updateFrequency(juce::MidiMessage::getMidiNoteInHertz(message.getNoteNumber()));
            break;
        }
        if (message.isNoteOff())
        {
            ampTarget = 0;
        }
    }
}

//==============================================================================
bool FMPluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *FMPluginProcessor::createEditor()
{
    return new FMPluginEditor(*this);
}

//==============================================================================
void FMPluginProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FMPluginProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new FMPluginProcessor();
}

double FMPluginProcessor::getDPhase(double freq, double sampleRate)
{
    double two_pi = 3.1415927 * 2;
    return (two_pi / sampleRate) * freq;
}
void FMPluginProcessor::updateFrequency(double newFreq)
{
    frequency = newFreq;
    dphase = getDPhase(frequency, getSampleRate());
    mod_dphase = getDPhase(frequency * mod_index, getSampleRate());
}

void FMPluginProcessor::setModIndex(double newIndex)
{
    *modIndexParam = (float)newIndex;
}
void FMPluginProcessor::setModDepth(double newDepth)
{
    *modDepthParam = (float)newDepth;
}
void FMPluginProcessor::setDroneMode(bool _dm)
{
    droneMode = _dm;
    if (droneMode)
    {
        ampTarget = 0.5;
        amp = 0.5;
    }
    else
    {
        ampTarget = 0;
    }
}

void FMPluginProcessor::addMidi(juce::MidiMessage msg, int sampleOffset)
{
    midiToProcess.addEvent(msg, sampleOffset);
}
