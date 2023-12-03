/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiMarkovProcessor::MidiMarkovProcessor()
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
      pitchModel{}, iOIModel{}, lastNoteOnTime{0}, elapsedSamples{0}, modelPlayNoteTime{0}, noMidiYet{true}, chordDetect{0}
{
  // set all note off times to zero 

  for (auto i=0;i<127;++i){
    noteOffTimes[i] = 0;
    noteOnTimes[i] = 0;
}
}

MidiMarkovProcessor::~MidiMarkovProcessor()
{
}

//==============================================================================
const juce::String MidiMarkovProcessor::getName() const
{
  return JucePlugin_Name;
}

bool MidiMarkovProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool MidiMarkovProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool MidiMarkovProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double MidiMarkovProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

int MidiMarkovProcessor::getNumPrograms()
{
  return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
            // so this should be at least 1, even if you're not really implementing programs.
}

int MidiMarkovProcessor::getCurrentProgram()
{
  return 0;
}

void MidiMarkovProcessor::setCurrentProgram(int index)
{
}

const juce::String MidiMarkovProcessor::getProgramName(int index)
{
  return {};
}

void MidiMarkovProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void MidiMarkovProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
  double maxIntervalInSamples = sampleRate * 0.05; // 50ms
  chordDetect = ChordDetector((unsigned long) maxIntervalInSamples); 
}

void MidiMarkovProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MidiMarkovProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void MidiMarkovProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
  ////////////
  // deal with MIDI

  // transfer any pending notes into the midi messages and
  // clear pending - these messages come from the addMidi function
  // which the UI might call to send notes from the piano widget
  if (midiToProcess.getNumEvents() > 0)
  {
    midiMessages.addEvents(midiToProcess, midiToProcess.getFirstEventTime(), midiToProcess.getLastEventTime() + 1, 0);
    midiToProcess.clear();
  }

  analysePitches(midiMessages);
  analyseDuration(midiMessages);
  analyseIoI(midiMessages);
  juce::MidiBuffer generatedMessages = generateNotesFromModel(midiMessages);

  // send note offs if needed  
  for (auto i = 0; i < 127; ++i)
  {
    if (noteOffTimes[i] > 0 &&
        noteOffTimes[i] < elapsedSamples)
    {
      juce::MidiMessage nOff = juce::MidiMessage::noteOff(1, i, 0.0f);
      generatedMessages.addEvent(nOff, 0);
      noteOffTimes[i] = 0;
    }
  }
  // now you can clear the outgoing buffer if you want
  midiMessages.clear();
  // then add your generated messages
  midiMessages.addEvents(generatedMessages, generatedMessages.getFirstEventTime(), -1, 0);

  elapsedSamples += buffer.getNumSamples();
}

//==============================================================================
bool MidiMarkovProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *MidiMarkovProcessor::createEditor()
{
  return new MidiMarkovEditor(*this);
}

//==============================================================================
void MidiMarkovProcessor::getStateInformation(juce::MemoryBlock &destData)
{
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void MidiMarkovProcessor::setStateInformation(const void *data, int sizeInBytes)
{
  // You should use this method to restore your parameters from this memory block,
  // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
  return new MidiMarkovProcessor();
}

void MidiMarkovProcessor::addMidi(const juce::MidiMessage& msg, int sampleOffset)
{
  midiToProcess.addEvent(msg, sampleOffset);
}

void MidiMarkovProcessor::resetMarkovModel()
{
  DBG("Resetting all models");
  pitchModel.reset();
  iOIModel.reset();
  noteDurationModel.reset();
}

void MidiMarkovProcessor::analyseIoI(const juce::MidiBuffer& midiMessages)
{
  // compute the IOI 
  for (const auto metadata : midiMessages){
      auto message = metadata.getMessage();
      if (message.isNoteOn()){   
          unsigned long exactNoteOnTime = elapsedSamples + message.getTimeStamp();
          unsigned long iOI = exactNoteOnTime - lastNoteOnTime;
          if (iOI < getSampleRate() * 2 && 
              iOI > getSampleRate() * 0.05){
            iOIModel.putEvent(std::to_string(iOI));
            // DBG("Note on at: " << exactNoteOnTime << " IOI " << iOI);

          }
          lastNoteOnTime = exactNoteOnTime; 
      }
  }
}
    
void MidiMarkovProcessor::analysePitches(const juce::MidiBuffer& midiMessages)
{
  for (const auto metadata : midiMessages)
  {
    auto message = metadata.getMessage();
    if (message.isNoteOn()){
      chordDetect.addNote(
            message.getNoteNumber(), 
            // add the offset within this buffer
            elapsedSamples + message.getTimeStamp()
        );
      if (chordDetect.hasChord()){
          std::string notes = 
              MidiMarkovProcessor::notesToMarkovState(
                  chordDetect.getChord()
              );
          DBG("Got notes from detector " << notes);
          pitchModel.putEvent(notes);
      }     
      noMidiYet = false;// bootstrap code
    }
  }
}

void MidiMarkovProcessor::analyseDuration(const juce::MidiBuffer& midiMessages)
{
  for (const auto metadata : midiMessages)
  {
    auto message = metadata.getMessage();
    if (message.isNoteOn())
    {
      noteOnTimes[message.getNoteNumber()] = elapsedSamples + message.getTimeStamp();
    }
    if (message.isNoteOff()){
      unsigned long noteOffTime = elapsedSamples + message.getTimeStamp();
      unsigned long noteLength = noteOffTime - 
                                  noteOnTimes[message.getNoteNumber()];
      noteDurationModel.putEvent(std::to_string(noteLength));
    }
  }
}


juce::MidiBuffer MidiMarkovProcessor::generateNotesFromModel(const juce::MidiBuffer& incomingNotes)
{

  juce::MidiBuffer generatedMessages{};
  if (isTimeToPlayNote(elapsedSamples)){
    if (!noMidiYet){ // not in bootstrapping phase 
      std::string notes = pitchModel.getEvent();
      unsigned int duration = std::stoi(noteDurationModel.getEvent(true));
      for (const int& note : markovStateToNotes(notes)){
          juce::MidiMessage nOn = juce::MidiMessage::noteOn(1, note, 0.5f);
          generatedMessages.addEvent(nOn, 0);
          noteOffTimes[note] = elapsedSamples + duration; 
      }
    }
    unsigned long nextIoI = std::stoi(iOIModel.getEvent());

    //DBG("generateNotesFromModel playing. modelPlayNoteTime passed " << modelPlayNoteTime << " elapsed " << elapsedSamples);
    if (nextIoI > 0){
      modelPlayNoteTime = elapsedSamples + nextIoI;
      // DBG("generateNotesFromModel new modelPlayNoteTime passed " << modelPlayNoteTime << "from IOI " << nextIoI);
    } 
  }
  return generatedMessages;
}

bool MidiMarkovProcessor::isTimeToPlayNote(unsigned long currentTime)
{
  // if (modelPlayNoteTime == 0){
  //   return false; 
  // }
  if (currentTime >= modelPlayNoteTime){
    return true;
  }
  else {
    return false; 
  }
}

// call after playing a note 
void MidiMarkovProcessor::updateTimeForNextPlay()
{

}

std::string MidiMarkovProcessor::notesToMarkovState(
               const std::vector<int>& notesVec)
{
std::string state{""};
for (const int& note : notesVec){
  state += std::to_string(note) + "-";
}
return state; 
}

std::vector<int> MidiMarkovProcessor::markovStateToNotes(
              const std::string& notesStr)
{
  std::vector<int> notes{};
  if (notesStr == "0") return notes;
  for (const std::string& note : 
           MarkovChain::tokenise(notesStr, '-')){
    notes.push_back(std::stoi(note));
  }
  return notes; 
}



