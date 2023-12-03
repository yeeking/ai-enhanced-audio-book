#include "../../tinywav/myk_tiny.h"
#include <string> 
#include <vector>
#include <iostream> 
    

/** work out norm factor given sent a and b coeffs*/
float getNormFactor(std::vector<float>& as, std::vector<float>& bs){
    float sum = 0;
    for (float b : bs){sum += b;}    
    for (float a : as){sum += a;}
    return 1/sum; 
}

int main(){

    std::vector<float> x = myk_tiny::loadWav("../../audio/drums_16bit.wav");
    // std::vector<float> x = {0.1, 0.2, 0.3, 0.2, 0.1, 0.5, 0.9, -0.1, 0.7, 0.1};
    
    std::vector<float> as = {0.5, 0.1, 0.2};
    std::vector<float> bs = {0.1, -0.7, 0.9};
    // might want to check as and bs are same length here 
    // pre-allocate the output
    std::vector<float> y(x.size(), 0.0f);
    // normalise factor
    float norm = getNormFactor(as, bs);
    
    for (auto n=as.size();n<x.size(); ++n){
        float yn = 0;
        // weighted sum of previous inputs
        for (auto bn=0;bn<bs.size();++bn){
            yn += bs[bn] * x[n - bn]; // acting on input x
        }
        // weighted sum of previous outputs
        for (auto an=0;an<as.size();++an){
            yn -= as[an] * y[n - an]; // acting on output y
        }
        y[n] = yn; 
        y[n] *= norm;
        //printf("x[%lu]=%f y[%lu]=%f\n", n, x[n], n, y[n]);
    }
    myk_tiny::saveWav(y, 1, 44100, "../../audio/iir_test.wav");

}

