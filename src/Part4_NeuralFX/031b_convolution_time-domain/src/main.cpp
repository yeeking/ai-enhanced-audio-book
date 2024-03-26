#include "../../tinywav/myk_tiny.h"

#include <string> 
#include <vector>


void amp(std::vector<float>& xs, float amp){
  for (int n=0;n<xs.size();++n){// iterate over signal x
    xs[n] = xs[n] * amp; 
  } 
}

std::vector<float> conv(std::vector<float> xs, std::vector<float> bs){
  std::vector<float> y(xs.size() + bs.size(), 0.0f);// output y
  int b_count = bs.size();
  for (int n=0;n<xs.size();++n){// iterate over signal x
    if (n % 10000 == 0){
      printf("%i of %lu \n", n, xs.size());
    }
    y[n] = 0;
    for (int b=0;b<n && b < b_count;++b){ // iterate over systems' coeffs b
      y[n] += bs[b] * xs[n-b];
    }
  }
  return y; 
}

float sumCoeffs(std::vector<float>& coeffs){
  float sum = 0.0;
  for (float& f : coeffs) sum += f;
  return sum;
}

/** apply a simple moving average filter */
void experiment1(){
  std::string xfile = "../../audio/drums_16bit.wav";
  std::string yfile = "../../audio/drums_expt1.wav";
  std::vector<float> x = myk_tiny::loadWav(xfile);
  // simple moving average low pass filter 
  std::vector<float> b = {0.5, 0.5, 0.5};
  std::vector<float> y = conv(x, b);
  amp(y, 1/sumCoeffs(b));// sum of 0.5, 0.5, 0.5
  myk_tiny::saveWav(y, 1, 44100, yfile);
}

/** apply an impule read from a file */
void experiment2(){
    std::string xfile = "../../audio/drums_16bit.wav";
    std::string bfile = "../../audio/short_imp.wav";
    std::string yfile = "../../audio/drums_expt2.wav";
    std::vector<float> x = myk_tiny::loadWav(xfile);
    std::vector<float> b = myk_tiny::loadWav(bfile);
    std::vector<float> y = conv(x, b);
    amp(y, 0.1); // 0.1 should do it :) 
   myk_tiny::saveWav(y, 1, 44100, yfile);
}

int main(){
  experiment1();
  experiment2();
}