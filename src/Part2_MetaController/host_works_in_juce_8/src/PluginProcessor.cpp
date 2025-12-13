/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginHostProcessor::PluginHostProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
,     audioProcGraph{new juce::AudioProcessorGraph()}

#endif

{
    // pluginFormatManager.registerBasicFormats();
    juce::addDefaultFormatsToManager(pluginFormatManager);
    // find the plugin format for VST3. We will need this when loading 
    // them later
    int currInd{0}, vstInd{0};
    for (const juce::AudioPluginFormat* f : pluginFormatManager.getFormats()){
        if (f->getName() == "VST3"){
        vstFormatInd = currInd;
        break;
        }
        currInd++; 
    }

    // make the i/o processors
    inputProc = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);    
    outputProc = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
    midiInputProc = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
    midiOutputProc = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode);
    
    audioProcGraph->enableAllBuses();
    inputProc->enableAllBuses();
    outputProc->enableAllBuses();

    // convert the io processors into nodes
    inputNode = audioProcGraph->addNode(std::move(inputProc));
    outputNode = audioProcGraph->addNode(std::move(outputProc));
    midiInNode = audioProcGraph->addNode(std::move(midiInputProc));
    midiOutNode = audioProcGraph->addNode(std::move(midiOutputProc));


}

PluginHostProcessor::~PluginHostProcessor()
{
}

//==============================================================================
const juce::String PluginHostProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginHostProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginHostProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginHostProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginHostProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginHostProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginHostProcessor::getCurrentProgram()
{
    return 0;
}

void PluginHostProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginHostProcessor::getProgramName (int index)
{
    return {};
}

void PluginHostProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PluginHostProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
  //  loadPlugin(juce::File{"/home/matthewyk/.vst3/Dexed.vst3"});
   audioProcGraph->prepareToPlay(sampleRate, samplesPerBlock);

  // loadPlugin(juce::File{"/home/matthewyk/.vst3/sine_plugin.vst3"});
  
   //loadPlugin(juce::File{"/home/matthewyk/.vst3/YeekFMBasic.vst3"});
   //loadPlugin(juce::File{"/home/matthewyk/.vst3/fm-superknob.vst3"});
}

void PluginHostProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    audioProcGraph->releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginHostProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PluginHostProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
     if (midiToProcess.getNumEvents() > 0){
      midiMessages.addEvents(midiToProcess, midiToProcess.getFirstEventTime(), midiToProcess.getLastEventTime()+1, 0);
      midiToProcess.clear();
    }
    if (pluginNode){
      audioProcGraph->processBlock(buffer, midiMessages);
    }


}

//==============================================================================
bool PluginHostProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginHostProcessor::createEditor()
{
    return new PluginHostEditor (*this);
}

//==============================================================================
void PluginHostProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PluginHostProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginHostProcessor();
}


void PluginHostProcessor::addMidi(juce::MidiMessage msg, int sampleOffset)
{
  midiToProcess.addEvent(msg, sampleOffset);
}

void PluginHostProcessor::loadPlugin(const juce::File& pluginFile)
{
  DBG("Loading a plugin  " << pluginFile.getFullPathName());
  suspendProcessing(true);

  pluginDescriptions.clear(); 
  bool added = knownPluginList.scanAndAddFile(pluginFile.getFullPathName(), 
                                    true, 
                                    pluginDescriptions, 
                                    *pluginFormatManager.getFormat(vstFormatInd));
  if (added) {DBG("Plugin can be loaded. Attempting load");}
  else {
    DBG("Plugin cannot be loaded, giving up");
    return; 
  }

  juce::String errorMsg{""};

  pluginInstance = pluginFormatManager.createPluginInstance(
    *pluginDescriptions[0], 
    getSampleRate(), getBlockSize(), errorMsg);
  
  if (errorMsg == "") {  DBG("Plugin probably loaded");}
  else {DBG("Plugin failed to load " << errorMsg);return;}

  addPluginToGraph();
  // plugin UI lives on the message thread, so resume audio afterwards
  suspendProcessing(false);

}

void PluginHostProcessor::addPluginToGraph()
{
    if (pluginNode) {
      audioProcGraph->removeNode(pluginNode);
    }
    pluginInstance->enableAllBuses();

    pluginNode = audioProcGraph->addNode (std::move (pluginInstance) );
    if (pluginNode == nullptr)
        return;
    auto* hostedProcessor = pluginNode->getProcessor();

    // connect audio input channels to the hosted plugin (only up to available channels)
    const auto inputChannels  = inputNode->getProcessor()->getTotalNumOutputChannels();
    const auto pluginIns      = hostedProcessor->getTotalNumInputChannels();
    const auto pluginOuts     = hostedProcessor->getTotalNumOutputChannels();
    const auto outputChannels = outputNode->getProcessor()->getTotalNumInputChannels();

    for (int ch = 0; ch < juce::jmin (inputChannels, pluginIns); ++ch)
        audioProcGraph->addConnection ({ { inputNode->nodeID, ch }, { pluginNode->nodeID, ch } });

    for (int ch = 0; ch < juce::jmin (pluginOuts, outputChannels); ++ch)
        audioProcGraph->addConnection ({ { pluginNode->nodeID, ch }, { outputNode->nodeID, ch } });

    // hook up midi in
    audioProcGraph->addConnection({
      {midiInNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}, 
      {pluginNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}});
 
    hostedProcessor->prepareToPlay(getSampleRate(), getBlockSize());
}


std::unique_ptr<juce::AudioProcessorEditor> PluginHostProcessor::createHostedPluginEditor()
{
    if (pluginNode == nullptr)
        return {};

    auto* processor = pluginNode->getProcessor();
    if (processor == nullptr)
        return {};

    if (processor->hasEditor())
        if (auto* ui = processor->createEditorIfNeeded())
            return std::unique_ptr<juce::AudioProcessorEditor> (ui);

    // fall back to the generic editor so the user can still interact
    return std::make_unique<juce::GenericAudioProcessorEditor> (*processor);
}
