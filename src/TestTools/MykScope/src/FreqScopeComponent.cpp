#include "FreqScopeComponent.h"

FreqScopeComponent::FreqScopeComponent() 
: scopeBuffer{1, 512}, bufLength{512}, forwardFFT{fftOrder}
{
    scopeBuffer.clear();
}

void FreqScopeComponent::paint (juce::Graphics& g)
{   

    // TODO: draw to an offscreen image only when the buffer gets updated.
    // just plot the buffer 
    g.setColour(juce::Colours::darkblue);
    g.fillAll();
    g.setColour(juce::Colours::orange);
    // auto rightHandEdge = getWidth() - 1;
    // auto imageHeight   = getHeight();

    juce::Range<float> maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData.data(), fftSize / 2); 

    float prevX{0}, prevY{0}, y{0}, x{0};
    float expScalar{1.1f}; // increase for a more 'log' scale..
    // count steps
    int totalSteps = 0;
    for (float fftInd = 1; fftInd < fftSize/2; fftInd *= expScalar){
        totalSteps ++;
    }
    float xStep = getWidth() / totalSteps;
    if (xStep < 1) xStep = 1;
    
    for (float fftInd = 1; fftInd < fftSize/2; fftInd *= expScalar){
        y = fftData[(unsigned long) fftInd -1];        
        if (y == 0) continue;
        
        y /= (maxLevel.getEnd() - maxLevel.getStart()); // normalise
        y += maxLevel.getStart();
        y *= getHeight();
        y = getHeight() - y;
        if (y < 0) y = 0;
        if (y >= getHeight()) y = getHeight() - 1;
        if (x < 0) x = 0;
        if (x >= getWidth()) x = getWidth() - 1;
        
        //if (prevX != x && prevY!= y){
            g.drawLine(prevX, prevY, x, y);
        //}
        prevY = y;
        prevX = x;
        x += xStep; 
    }

}
void FreqScopeComponent::updateBuffer(const juce::AudioBuffer<float>& buffer, float gain)
{


    // work out the bufLength which is the number of samples we will copy
    bufLength = buffer.getNumSamples();
    if (bufLength > scopeBuffer.getNumSamples()){
        bufLength = scopeBuffer.getNumSamples();
    }
    scopeBuffer.copyFrom(0, //int destChannel,
                   0, //int destStartSample,
                   buffer, //const AudioBuffer& source,
                   0, //int sourceChannel,
                   0, //int sourceStartSample,
                   bufLength);//int numSamples);
    scopeBuffer.applyGain(gain);
    // now do the FFT copy business
    auto* channelData = scopeBuffer.getReadPointer (0);
    for (auto i = 0; i < bufLength; ++i){
        pushNextSampleIntoFifo (channelData[i]);
    }
    // now I think I can fft it 
    
    //forwardFFT.performFrequencyOnlyForwardTransform (fftData.data()); 
}

// thanks https://docs.juce.com/master/tutorial_simple_fft.html
void FreqScopeComponent::pushNextSampleIntoFifo (float sample)  noexcept
{
    // if the fifo contains enough data, set a flag to say
    // that the next line should now be rendered..
    if (fifoIndex == fftSize)       // [8]
    {
        if (! nextFFTBlockReady)    // [9]
        {
            std::fill (fftData.begin(), fftData.end(), 0.0f);
            std::copy (fifo.begin(), fifo.end(), fftData.begin());
            nextFFTBlockReady = true;
            forwardFFT.performFrequencyOnlyForwardTransform (fftData.data());                   
            nextFFTBlockReady = false;
        }

        fifoIndex = 0;
    }

    fifo[(size_t) fifoIndex++] = sample; // 

}
