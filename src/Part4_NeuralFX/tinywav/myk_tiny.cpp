
#include "myk_tiny.h"
#include <iostream> 

std::vector<float> myk_tiny::loadWav(const std::string& filename){
    TinyWav twReader;
    tinywav_open_read(&twReader, filename.c_str(), TW_INLINE); 
    size_t bufferSize = twReader.numChannels * twReader.numFramesInHeader;   
    // dynamically allocate buffer on the heap 
    float* mainBuffer = new float[bufferSize]; 
    size_t subSize = 44100; // max read per iteration 
    float* subBuffer = new float[subSize];
    
    for (auto offset = 0; offset < bufferSize; offset += subSize){
        size_t samplesRead = tinywav_read_f(&twReader, subBuffer, subSize);
        // copy samples read to the main buffer
        // std::cout << "Samples read " << samplesRead << " of " <<bufferSize << std::endl; 
        if (samplesRead == 0) break;
        std::copy(subBuffer, subBuffer + samplesRead, mainBuffer + offset);
        // std::cout << (mainBuffer + offset)[0] << std::endl;

    }
    // iterate over the buffer, reading up to 4096 samples  
    tinywav_close_read(&twReader);
    std::vector<float> vBuffer(mainBuffer, mainBuffer + bufferSize);
    // do the housework
    delete[] mainBuffer;
    delete[] subBuffer;
    return vBuffer; 
}

void myk_tiny::saveWav( std::vector<float>& buffer, const int channels, const int sampleRate, const std::string& filename){
    TinyWav twWriter;
    size_t subSize = 44100; // max write per iteration 

    tinywav_open_write(&twWriter, channels, sampleRate, TW_INT16, TW_INLINE, filename.c_str());
    for (auto offset = 0; offset < buffer.size(); offset += subSize){
        int remainingSamples = buffer.size() - offset;
        if (remainingSamples == 0) break;
        int samplesToWrite = (remainingSamples < subSize ? remainingSamples : subSize);
        int samplesWritten = tinywav_write_f(&twWriter, (buffer.data() + offset), samplesToWrite);
    }
    tinywav_close_write(&twWriter);
}
