/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
// #include "MusicCircleClient.h"
// #include "LogginManager.h"

enum class PluginState{ready, recording, mcLoggin, mcUploadAudio, mcUploadMidi, error};

//==============================================================================
/** PluginProcessorEditor: Handle Graphical User Interface and calls to LoginManager
*/
class DinvernoAudioMidiRecorderPluginProcessorEditor  : public AudioProcessorEditor,
                                        public Button::Listener
                                        // public LogginListener
{
public:
    DinvernoAudioMidiRecorderPluginProcessorEditor (DinvernoAudioMidiRecorderPluginProcessor&);
    ~DinvernoAudioMidiRecorderPluginProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    // Listener interface for buttons
    void buttonClicked (Button* button) override;
    
    //==============================================================================
    /** LogginListener callback */
    // void musicCircleEvent(MusicCircleEvent event) override;
    
    bool recording = false;
    bool uploading = false;
    PluginState state;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DinvernoAudioMidiRecorderPluginProcessor& audioProcessor;

    // GUI Objects
    TextButton recordButton;
    void updateButton(PluginState currentState, String msg);
    
    // Recording
    File audioRecordingFile;
    File midiRecordingFile;
    
    //Music Circle
    // LogginManager loggin;
    std::string default_username{"csys2"};
    std::string default_password{"test123"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DinvernoAudioMidiRecorderPluginProcessorEditor)
};
