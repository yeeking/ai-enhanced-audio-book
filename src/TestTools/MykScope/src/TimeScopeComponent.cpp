#include "TimeScopeComponent.h"

TimeScopeComponent::TimeScopeComponent() 
: scopeBuffer{1, 512}, bufLength{512}
{
    scopeBuffer.clear();
}

void TimeScopeComponent::paint (juce::Graphics& g)
{   
    // just plot the buffer 
    g.setColour(juce::Colours::darkblue);
    g.fillAll();
    g.setColour(juce::Colours::orange);

    float max{0};
    int skip = bufLength / getWidth();
    float steps = bufLength / skip; 
    float  xSkip = getWidth() / steps; 
    if (skip < 1) skip = 1;
    float x{0}, y{0}, prevX{0}, prevY{0};
    auto* buffData = scopeBuffer.getReadPointer(0);
   for (int ind=0;ind<bufLength;ind+=skip){
    
   // for (int ind=0;ind<scopeBuffer.getNumSamples();ind+=skip){
        float sample = buffData[ind]; 
        
        if (fabs(sample) > max) max = fabs(sample); 
        y = (sample * 0.5f) + 0.5f; // 0-1
        //DBG("d:" << sample << " px " << prevX << " py " << prevY << " x " << x << " y " << y);
        y *= (float) getHeight();
        // avoid going too high
        y = juce::jlimit<float>(0, getHeight()-1, y);
        g.drawLine(prevX, prevY, x, y);
        prevX = x;
        prevY = y; 
        x += xSkip;

    }

   // DBG("max sq sample = " << max);
}
void TimeScopeComponent::updateBuffer(const juce::AudioBuffer<float>& buffer, float gain)
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
    // auto* buffRead = buffer->getReadPointer(0);
    // auto* buffWrite = scopeBuffer.getWritePointer(0);

    // for (int ind = 0;ind<bufLength; ++ind){
    //     buffWrite[ind] = buffRead[ind];
    // }

}