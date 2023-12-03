#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       )
{
// linux
//   lstmModel = torch::jit::load("/home/matthewyk/src/ai-enhanced-audio/ai-audio-book-code/src/Part4_NeuralFX/036f_lstm-blocks-memory/my_lstm_with_state.pt");
  // mac
    lstmModel = torch::jit::load("/Users/matthewyk/src/ai-audio-book/ai-audio-book-code/src/Part4_NeuralFX/036f_lstm-blocks-memory/my_lstm_with_state.pt");
  lstmState = getRandomStartState(1, 1);  
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //juce::ignoreUnused (sampleRate, samplesPerBlock);
    inBuffer.resize((size_t)samplesPerBlock);
    outBuffer.resize((size_t)samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* input = buffer.getReadPointer (channel);
        std::copy(input, input + inBuffer.size(), inBuffer.begin());
        processBlockState(lstmModel, lstmState, inBuffer, outBuffer, buffer.getNumSamples());
        auto* output = buffer.getWritePointer (channel);
        std::copy(outBuffer.begin(), outBuffer.begin() + inBuffer.size(), output);
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}


LSTMState AudioPluginAudioProcessor::getRandomStartState(int numLayers, int hiddenSize){
    torch::Tensor h0 = torch::randn({numLayers, hiddenSize});
    torch::Tensor c0 = torch::randn({numLayers, hiddenSize});
    LSTMState state = c10::ivalue::Tuple::create({h0, c0}); 
    return state;
}

LSTMState AudioPluginAudioProcessor::processBlockState(torch::jit::script::Module& model, const LSTMState& state, std::vector<float>& inBlock, std::vector<float>& outBlock, int numSamples){

  torch::Tensor in_t = torch::from_blob(inBlock.data(), {static_cast<int64_t>(numSamples)});
  in_t = in_t.view({-1, 1});  
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(in_t);
  inputs.push_back(state);
  
  torch::jit::IValue out_ival = model.forward(inputs);
  // copy to the outBlock
  auto out_elements = out_ival.toTuple()->elements();
  torch::Tensor out_t = out_elements[0].toTensor();
  out_t = out_t.view({-1});
  float* data_ptr = out_t.data_ptr<float>();
  std::copy(data_ptr, data_ptr+inBlock.size(), outBlock.begin());
  // now retain the state
  return out_elements[1].toTuple();
}
