// C++ helpers for tinywav

#include "tinywav/tinywav.h"

// couple of functions added by myk :)
std::vector<float> tinywav_loadWav(std::string filename){
    TinyWav twReader;
    tinywav_open_read(&twReader, filename.c_str(), TW_INLINE); 
    int bufferSize = twReader.numChannels * twReader.numFramesInHeader;
    float buffer[bufferSize];

    int samplesRead = tinywav_read_f(&twReader, buffer, twReader.numFramesInHeader);
    tinywav_close_read(&twReader);
    std::vector<float> vBuffer(buffer, buffer + bufferSize);
    printf("Read %lu samples\n", vBuffer.size());
    return vBuffer; 
}

void tinywav_myk_tiny::saveWav(std::vector<float>& buffer, int channels, int sampleRate, std::string filename){
    printf("myk_tiny::saveWav: saving buffer of size %lu \n", buffer.size());
    TinyWav twWriter;
    tinywav_open_write(&twWriter, channels, sampleRate, TW_INT16, TW_INLINE, filename.c_str());
    int samplesWritten = tinywav_write_f(&twWriter, buffer.data(), buffer.size() / channels);
    printf("Wrote %i samples\n", samplesWritten);
    tinywav_close_write(&twWriter);
}

