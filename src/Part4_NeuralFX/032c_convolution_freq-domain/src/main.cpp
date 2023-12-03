#include "../../tinywav/myk_tiny.h"
#include <string> 
#include <vector>
#include <iostream>
#include "simple_fft/fft_settings.h"
#include "simple_fft/fft.h"

#include <chrono>

unsigned long getHighestPower2(unsigned long val){
  unsigned int shift{0};
  unsigned long max{1};
  while (max << shift <= val) ++shift;
  // shift goes one too high
  return (max << (shift-1) );
}

/**
 * FFT the sent 1D array of float samples
 * returns a complex spectrum 
*/
std::vector<complex_type> doFFT(std::vector<real_type>& samples){
  unsigned long max = getHighestPower2(samples.size()); // highest power of 2
  std::vector<complex_type> data_out(max);
  const char * error_description = 0;

  bool res = simple_fft::FFT(samples, data_out, max,
                          error_description);
  if (!res){
    std::cout << "doFFT: error message: " << error_description << std::endl;
  }
  return data_out; 
}


std::vector<float> appendZeroesToLength(std::vector<float> input, unsigned long length){
  // std::cout << "appendZeroesToLength got size " << input.size() << " want size " << length << std::endl; 
  if (input.size() == length) return input; // do nothing! 
  input.resize(length);
  for (auto ind=input.size();ind<length;++ind){
    input[ind] = 0;
  }
  // std::cout << "appendZeroesToLength final size " << input.size() << " wanted size " << length << std::endl; 
  return input; 
}

/**
 * IFFT the sent complex spectrum 
 * returns a 1D array if real values
*/
std::vector<real_type>  doIFFT(std::vector<complex_type>& complex_spectrum){
  std::vector<real_type> samples(complex_spectrum.size());
  const char * error_description = 0;
  // do in-place 1d IFFT
  bool res = simple_fft::IFFT(complex_spectrum, complex_spectrum.size(), error_description);
  if (!res){
    std::cout << "doIFFT: error message: " << error_description << std::endl;
  }
  // now copy the real part into the samples vector
  for (int i=0;i<complex_spectrum.size();++i){
    samples[i] = complex_spectrum[i].real();
  }

  return samples; 
}
/**
 * computes the point products of x and y, AKA the convolution
*/
std::vector<complex_type> convolveSpectra(std::vector<complex_type>& x, std::vector<complex_type>& y){
  std::vector<complex_type> res(x.size());
  for (auto i=0;i<res.size();++i){
    res[i] = x[i]*y[i];
  }
  return res; 
}

std::vector<float> amp(std::vector<float> xs, float amp){
  
  std::vector<float> y(xs.size(), 0.0f);// output y
  for (int n=0;n<xs.size();++n){// iterate over signal x
    y[n] = xs[n] * amp; 
  }
  return y; 
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

int main(){
    std::string xfile = "../../audio/drums_16bit.wav";
    //std::string bfile = "../../audio/short_imp.wav";
    std::string bfile = "../../audio/church_16bit.wav";
  
    std::string yfile = "../../audio/tinyfft_output.wav";
    std::string xReffile = "../../audio/tinyfft_input.wav";
    
    std::cout << "Loading signal "<< xfile << " system " << bfile << std::endl;
    std::vector<float> x = myk_tiny::loadWav(xfile);
    std::vector<float> b = myk_tiny::loadWav(bfile);
    std::cout << "Files loaded " << std::endl;
    if (x.size() > b.size() ) {// input signal is longer that impulse response, zero pad imp at end
      b = appendZeroesToLength(b, x.size());
    }
    if (b.size() > x.size() ) {// input system is longer that impulse response, zero pad imp at end
      x = appendZeroesToLength(x, b.size());
    }
    
    for (int i=0;i<20;i++){
    // start timer 
    auto start = std::chrono::high_resolution_clock::now();
 
    // go to the frequency domain
    std::vector<complex_type> specX = doFFT(x);
    std::vector<complex_type> specB = doFFT(b);
    float file_len_seconds = specX.size() / 44100.0f;    
  
    std::vector<complex_type> convSpec = convolveSpectra(specX, specB);
    std::vector<float> y = doIFFT(convSpec);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    // // duration is how long it took to process the entire audio file. 
    // float dur_per_second = (duration.count()/1000.0f)  / file_len_seconds;
    // float dur_per_coefficient = dur_per_second / b.size(); 
    std::cout << "With IR len "<< y.size() << " conved " << file_len_seconds << "s file in " << (duration.count() / 1000.0f) << "s" << std::endl;
    }
    // save the processed file to disk
    // also save the ifft'd input for reference
    // std::vector<float> x2 = doIFFT(specX);
    // std::cout << "Saving result to "<< yfile << " ref input to " << xReffile << std::endl;
    // myk_tiny::saveWav(y, 1, 44100, yfile);   
    //myk_tiny::saveWav(x2, 1, 44100, xReffile);   
}
