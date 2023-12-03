#include "../../tinywav/myk_tiny.h"
#include <string> 
#include <vector>
#include <iostream> 
#include <algorithm>
#include <cmath>

float relu(float x) {
    return std::max(0.0f, x);
}

float sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

float clip(float input, float clip_value) {
    if (std::abs(input) > clip_value) {
        return (input < 0) ? -1.0f : 1.0f;
    }
    return input;
}

int main(){
   std::vector<float> audio = myk_tiny::loadWav("../../audio/drums_16bit.wav");
   for (auto i=0;i<audio.size();++i){
    audio[i] = relu(audio[i]);
   }
   myk_tiny::saveWav(audio, 1, 44100, "../../audio/waveshaped.wav");

}

