/**
 * Scope components: time domain plotter and frequency domain plotter
 * 
 */
#pragma once

#include <JuceHeader.h>

class TimeScopeComponent : public juce::Component {
    public: 
    TimeScopeComponent();
    void paint (juce::Graphics&) override;
    // copies the sent buffer into the stored internal buffer
    virtual void updateBuffer(const juce::AudioBuffer<float>& buffer, float gain);
   
    private:
        juce::AudioBuffer<float> scopeBuffer;
        int bufLength;
};


