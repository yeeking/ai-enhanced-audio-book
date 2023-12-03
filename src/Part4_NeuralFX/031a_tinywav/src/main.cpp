#include "../../tinywav/myk_tiny.h"

#include <string> 
#include <vector>
#include <iostream> 

int main(){
    std::vector<float> audio =  myk_tiny::loadWav("../audio/toby.wav");
    myk_tiny::saveWav(audio, 1, 44100, "../audio/testtiny.wav");
}

