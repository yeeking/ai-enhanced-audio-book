/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DinvernoAudioMidiRecorderPluginProcessorEditor::DinvernoAudioMidiRecorderPluginProcessorEditor (DinvernoAudioMidiRecorderPluginProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 100);
    
    // Music Circle Login Manager
    // loggin.addEventListener(this);
   
    // Setup Record Button
    addAndMakeVisible(recordButton);
    recordButton.addListener(this);
    
    // StateMachine
    state = PluginState::ready;
    updateButton(state, "Start Recording");
    
    //recordButton.setButtonText("Start Recording");
    //recordButton.setColour(TextButton::buttonColourId,Colours::green);
    
}

DinvernoAudioMidiRecorderPluginProcessorEditor::~DinvernoAudioMidiRecorderPluginProcessorEditor()
{
}

//==============================================================================
void DinvernoAudioMidiRecorderPluginProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void DinvernoAudioMidiRecorderPluginProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // GUI Components
    recordButton.setBounds(0,0,getWidth(),getHeight());
}

void DinvernoAudioMidiRecorderPluginProcessorEditor::updateButton(PluginState currentState, String msg)
{
    recordButton.setButtonText(msg);
    switch(currentState)
    {
        case PluginState::ready:
            recordButton.setColour(TextButton::buttonColourId,Colours::green);
            break;
        case PluginState::recording:
            recordButton.setColour(TextButton::buttonColourId,Colours::red);
            break;
        case PluginState::mcLoggin:
            recordButton.setColour(TextButton::buttonColourId,Colours::blue);
            break;
        case PluginState::mcUploadAudio:
            recordButton.setColour(TextButton::buttonColourId,Colours::blue);
            break;
        case PluginState::mcUploadMidi:
            recordButton.setColour(TextButton::buttonColourId,Colours::blue);
            break;
        case PluginState::error:
            recordButton.setColour(TextButton::buttonColourId,Colours::orange);
            break;
    }
}

void DinvernoAudioMidiRecorderPluginProcessorEditor::buttonClicked (Button* button)
{
    //if (!recording && !uploading){
    if (state == PluginState::ready){
        // ReadyState + ButtonPress = Start Recording
        
        // Setup Recording Driectory
        auto docsDir = File::getSpecialLocation (File::userMusicDirectory); 
        auto parentDir = File(docsDir.getFullPathName()+"/AudioMidiRecordings" );
        parentDir.createDirectory();
        
        // Date-Time for FileNames
        Time dateTime = Time::getCurrentTime();
        String dateTimeFormatted = dateTime.formatted("%Y-%m-%d_%H-%M-%S");
        String programMode = audioProcessor.getProgramName(audioProcessor.getCurrentProgram());
        String fileName = dateTimeFormatted+"_"+programMode;
        
        // Audio Recording File (Swap between .wav and .ogg formats here)
        audioRecordingFile = parentDir.getNonexistentChildFile(fileName, ".wav");    //Wav Audio File Format
        // audioRecordingFile = parentDir.getNonexistentChildFile(fileName, ".ogg");  //OGG Audio File Format
        
        // Midi Recording File (same name as audio file - will overwrite if file exists)
        midiRecordingFile = parentDir.getChildFile(audioRecordingFile.getFileNameWithoutExtension()+".mid");
        
        // Tell Audio Processor to start recording
        audioProcessor.startRecordingAudio(audioRecordingFile);
        audioProcessor.startRecordingMidi(midiRecordingFile);
        
        // Update GUI
        //recordButton.setButtonText("Stop Recording");
        //recordButton.setColour(TextButton::buttonColourId,Colours::red);
        //recording = true;
    
        // Update State and GUI
        state = PluginState::recording;
        updateButton(state, "Stop Recording");

    
    }else if (state == PluginState::recording){
        // RecordingState + ButtonPress = Stop Recording, Start Uploading
        
        // Tell Audio Processor to Stop Recording
        audioProcessor.stopRecordingAudio();
        audioProcessor.stopRecordingMidi();
        
        // Update GUI
        //recordButton.setButtonText("Uploading...\n"+audioRecordingFile.getFileNameWithoutExtension());
        //recordButton.setColour(TextButton::buttonColourId,Colours::blue);
        //uploading = true;
        //recording = false;
        
        // Update State and GUI
        // state = PluginState::mcLoggin;
        // updateButton(state, "Logging into\nMusicCircle...");
        
        state = PluginState::ready;
        updateButton(state, "Start Recording");
        // tell the user where the files are
        AlertWindow::showMessageBoxAsync(
                juce::MessageBoxIconType::InfoIcon, 
                "", 
                "Files saved to "+audioRecordingFile.getFullPathName(), 
                "OK");   


        // Music Circle (loggin for now)
        // loggin.loginToMC(default_username, default_password);

    }else if (state == PluginState::mcLoggin){
        //UploadState + ButtonPress = DoNothing
    }else if (state == PluginState::mcUploadAudio){
        //UploadState + ButtonPress = DoNothing
    }else if (state == PluginState::mcUploadMidi){
        //UploadState + ButtonPress = DoNothing
    }
    else if (state == PluginState::error){
        //ErrorState + ButtonPress = ReadyState
        state = PluginState::ready;
        updateButton(state, "Start Recording");
    }
}

// void DinvernoAudioMidiRecorderPluginProcessorEditor::musicCircleEvent(MusicCircleEvent event)
// {
//     const MessageManagerLock mmLock;

//     /*
//     // lock the message thread
//     // as this func is called from another thread
//     // and we get assertion errors otherwise
//     const MessageManagerLock mmLock;
//     */
//     juce::String msg = "";
     
//     switch(event){
//         case MusicCircleEvent::login_succeeded:
//             // Successful Login
//             // msg  << "\nLogged in. user id " << loggin.getUserId();
            
//             if (state == PluginState::mcLoggin){
//                 // Login Successful: Begin Audio File Upload
//                 state = PluginState::mcUploadAudio;
//                 updateButton(state, "Uploading Audio...\n"+audioRecordingFile.getFileNameWithoutExtension());
                
//                 // loggin.postMedia(audioRecordingFile.getFullPathName().toStdString(), [this](int result){
//                     std::cout << "MainComponent::recordingComplete postMedia callback result " << result << std::endl;
//                     //loggin.sendQueuedAnnotations();
//                 });
//             }
//             break;
//         case MusicCircleEvent::login_failed:
//             // Login Failed: Show error on screen
//             msg << "\nFailed to login with user " << default_username;
            
//             // Update state
//             state = PluginState::error;
//             updateButton(state, "MusicCircle Login Failed...\nClick here to try again");
            
//             break;
//         case MusicCircleEvent::media_upload_succeeded:
//             // Upload Succeeded: Begin Midi Upload
//             msg << "\nMedia upload succeeded. ";// + (usernameField.getText());
            
//             if (state == PluginState::mcUploadAudio){
//                 // Update GUI
//                 state = PluginState::mcUploadMidi;
//                 updateButton(state, "Uploading Midi...\n"+midiRecordingFile.getFileNameWithoutExtension());
                
//                 //TODO: Midi File Upload here
//                 // loggin.postMediaFile(midiRecordingFile.getFullPathName().toStdString(), [this](int result){
//                 //     std::cout << "MainComponent::uploadComplete postMediaFile callback result " << result << std::endl;
//                 // });
                
//             }else if (state == PluginState::mcUploadMidi){
//                 // Update GUI
//                 state = PluginState::ready;
//                 updateButton(state, "Start Recording");
//             }
            
//             break;
//         case MusicCircleEvent::media_upload_failed:
//             msg << "\nFailed to upload recording";
            
//             // Update state
//             state = PluginState::error;
//             updateButton(state, "MusicCircle Upload Failed...\nClick here to try again");
            
//             break;
//         case MusicCircleEvent::logout_succeeded:
//             msg << "\nLogged out user " << default_username;
//             break;
//         case MusicCircleEvent::logout_failed:
//             // msg << "\nFailed to logout user id " << loggin.getUserId();
//             break;
            
//     }

//     std::cout << msg << std::endl;
// }
