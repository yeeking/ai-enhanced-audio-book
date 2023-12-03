/**
 * Scope components: time domain plotter and frequency domain plotter
 * 
 */
#pragma once

#include <JuceHeader.h>
#include "TimeScopeComponent.h"

class FreqScopeComponent : public juce::Component {
    public: 
    FreqScopeComponent();
    void paint (juce::Graphics&) override;
    // copies the sent buffer into the stored internal buffer
    virtual void updateBuffer(const juce::AudioBuffer<float>& buffer, float gain);
   
    private:
      juce::AudioBuffer<float> scopeBuffer;
      int bufLength;
      
       // after https://docs.juce.com/master/tutorial_simple_fft.html
      static constexpr auto fftOrder = 10;                
      static constexpr auto fftSize  = 1 << fftOrder;  
      juce::dsp::FFT forwardFFT;                          
      juce::Image spectrogramImage;
      std::array<float, fftSize> fifo;                    
      std::array<float, fftSize * 2> fftData;             
      int fifoIndex = 0;                                  
      bool nextFFTBlockReady = false;  

      void pushNextSampleIntoFifo (float sample) noexcept;
                      
};



