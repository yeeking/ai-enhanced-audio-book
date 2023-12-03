#include "../../tinywav/myk_tiny.h"

#include <string> 
#include <vector>
#include <chrono>
#include <iostream> 


void amp(std::vector<float>& xs, float amp){
  for (int n=0;n<xs.size();++n){// iterate over signal x
    xs[n] = xs[n] * amp; 
  } 
}

std::vector<float> conv(std::vector<float> xs, std::vector<float> bs){
  std::vector<float> y(xs.size() + bs.size(), 0.0f);// output y
  int b_count = bs.size();
  for (int n=0;n<xs.size();++n){// iterate over signal x
//    if (n % 10000 == 0){
//      printf("%i of %lu \n", n, xs.size());
//    }
    y[n] = 0;
    for (int b=0;b<n && b < b_count;++b){ // iterate over systems' coeffs b
      y[n] += bs[b] * xs[n-b];
    }
  }
  return y; 
}

/** apply a simple moving average filter */
void experiment1(){
  std::string xfile = "../../audio/drums_16bit.wav";
  std::string yfile = "../../audio/drums_expt1.wav";
  std::vector<float> x = myk_tiny::loadWav(xfile);
  // simple moving average low pass filter 
  std::vector<float> b = {0.5, 0.5, 0.5};
  std::vector<float> y = conv(x, b);
  amp(y, 1/4.5);// sum of 0.5, 0.5, 0.5
  myk_tiny::saveWav(y, 1, 44100, yfile);
}

/** apply a simple moving average filter */
void experiment2(){
  std::string xfile = "../../audio/drums_16bit.wav";
  std::string yfile = "../../audio/drums_expt1.wav";
  std::vector<float> x = myk_tiny::loadWav(xfile);
  float file_len_seconds = x.size() / 44100.0f; // how many seconds in the file 
  // simple moving average low pass filter
  for (float i=63536;i<200000;i+=1000){ 
     std::vector<float> b{};
     for (int j=0;j<i;++j){
         b.push_back(0.5);
     }
     auto start = std::chrono::high_resolution_clock::now();
     std::vector<float> y = conv(x, b);
     auto stop = std::chrono::high_resolution_clock::now();
     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
     // duration is how long it took to process the entire audio file. 
     float dur_per_second = (duration.count()/1000.0f)  / file_len_seconds;
     float dur_per_coefficient = dur_per_second / b.size(); 
     std::cout << "With IR len "<< b.size() << " conved " << file_len_seconds << "s file in " << (duration.count() / 1000.0f) << "s" << std::endl;
   }
//  amp(y, 1/4.5);// sum of 0.5, 0.5, 0.5
//  myk_tiny::saveWav(y, 1, 44100, yfile);
}



/** apply an impule read from a file */
void experiment3(){
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
//  experiment1();
  experiment2();
//  experiment3();

}
