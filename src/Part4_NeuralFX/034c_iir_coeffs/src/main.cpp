#include "../../tinywav/myk_tiny.h"
#include <string> 
#include <vector>
#include <iostream> 
#include <cmath>


/** work out norm factor given sent a and b coeffs*/
float getNormFactor(std::vector<float>& as, std::vector<float>& bs){
    float sum = 0;
    for (float b : bs){sum += b;}    
    for (float a : as){sum += a;}
    return 1/sum; 
}

int main(){

    std::vector<float> x = myk_tiny::loadWav("../../audio/drums_16bit.wav");
    // low pass butterworth from python
    // b,a = signal.butter(2, 1000, 'low', fs=44100)
    // std::vector<float> bs = {1.   ,      -1.79909641 , 0.8175124 };
    // std::vector<float> as = { 0.9041522 , -1.80830441 , 0.9041522};
    // hipass butterworth from python
    // b,a = signal.butter(2, 1000, 'hp', fs=44100)
    std::vector<float> bs = { 1.  ,       -1.79909641 , 0.8175124 };
    std::vector<float> as = {0.004604 ,0.009208 ,0.004604};
    std::vector<float> y(x.size(), 0.0f);
    //normalise factor
    float norm = getNormFactor(as, bs);
    
    for (auto n=as.size();n<x.size(); ++n){
        float yn = 0;
        // weighted sum of previous inputs
        for (auto bn=0;bn<bs.size();++bn){
            //printf("b[%lu] = %f \n", bn, bs[bn]);
            yn += bs[bn] * x[n - bn]; // acting on input x
        }
        // weighted sum of previous outputs
        for (auto an=0;an<as.size();++an){
            //printf("a[%lu] = %f \n", an, as[an]);
            yn += as[an] * y[n - an]; // acting on output y
        }
        y[n] = yn; 
        //y[n] *= norm;
        //printf("x[%lu]=%f y[%lu]=%f\n", n, x[n], n, y[n]);
    }
    myk_tiny::saveWav(y, 1, 44100, "../../audio/iir_test_hi.wav");

}

