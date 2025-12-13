// /*
//   ==============================================================================

//     PluginWindow.cpp
//     Created: 25 Sep 2022 3:30:54pm
//     Author:  matthewyk

//   ==============================================================================
// */

// #include "PluginWindow.h"


// PluginWindow::PluginWindow (juce::AudioProcessorEditor* editor)
//     : DocumentWindow ("plugin UI",
//                       juce::LookAndFeel::getDefaultLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId),
//                       juce::DocumentWindow::minimiseButton | juce::DocumentWindow::closeButton)
//     //  activeWindowList (windowList),
// {
//     setSize (400, 300);

//     if (editor) 
//     {
//         setContentOwned (editor, true);
//         setResizable (editor->isResizable(), false);
//     }

//     // TODO - do something clever here for window placement
//     setTopLeftPosition(100, 100);

//     setVisible (true);
// }

// PluginWindow::~PluginWindow() 
// {
//     clearContentComponent();
// }

// void PluginWindow::closeButtonPressed()
// {
//   // brutal but it should work
//   //delete this;
//   listener->pluginCloseButtonClicked();
// }

// void PluginWindow::addPluginWindowListener(PluginWindowListener* _listener)
// {
//   this->listener = _listener; 
// }


/*
  ==============================================================================

    PluginWindow.cpp
    Created: 25 Sep 2022 3:30:54pm
    Author:  matthewyk

  ==============================================================================
*/

#include "PluginWindow.h"


PluginWindow::PluginWindow (std::unique_ptr<juce::AudioProcessorEditor> editor)
    : DocumentWindow ("plugin UI",
                      juce::LookAndFeel::getDefaultLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId),
                      juce::DocumentWindow::minimiseButton | juce::DocumentWindow::closeButton)
    //  activeWindowList (windowList),
{
    if (editor)
    {
        auto* ownedEditor = editor.release();
        setContentOwned (ownedEditor, true);
        setResizable (ownedEditor->isResizable(), false);
        setSize (ownedEditor->getWidth(), ownedEditor->getHeight());
    }
    else
    {
        setSize (400, 300);
    }

    // TODO - do something clever here for window placement
    setTopLeftPosition(100, 100);
    setVisible (true);
}

PluginWindow::~PluginWindow() 
{
    clearContentComponent();
}

void PluginWindow::closeButtonPressed()
{
    if (listener){
        listener->pluginCloseButtonClicked();
    }
}
