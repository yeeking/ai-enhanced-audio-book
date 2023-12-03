#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>


class PluginWindowListener{
  public:
    PluginWindowListener(){}
   ~PluginWindowListener(){}
    virtual void pluginCloseButtonClicked() = 0;
};


class PluginWindow  : public juce::DocumentWindow
{
public:
    PluginWindow (juce::AudioProcessorEditor* editor);
    ~PluginWindow() override;
    void closeButtonPressed() override;
    void addPluginWindowListener(PluginWindowListener*  _listener){listener = _listener;}

private:
    PluginWindowListener*  listener{nullptr};        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

