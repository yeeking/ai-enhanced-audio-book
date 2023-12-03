#include <torch/script.h> // One-stop header.
#include "../../tinywav/myk_tiny.h"

#include <iostream>
#include <memory>
#include <vector>

void processBlock(  torch::jit::script::Module& model, std::vector<float>& inBlock, std::vector<float>& outBlock, int numSamples){

  torch::Tensor in_t = torch::from_blob(inBlock.data(), {static_cast<int64_t>(numSamples)});
  // view and reshape can both reshape but view assumes the data is contiguous in memory
  // which it should be in a vector (hopefully, eh? maybe safer to use reshape which will 
  // reformat the data in memory if necessary... )
  in_t = in_t.view({1, -1, 1});// batch size,seq length, channels - the -1 lets torch figure out the sequence length
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(in_t);
  torch::jit::IValue out_ival = model.forward(inputs);
  // copy to the outBlock
  torch::Tensor out_t = out_ival.toTensor();
  
  out_t = out_t.view({-1});
  float* data_ptr = out_t.data_ptr<float>();
  std::copy(data_ptr, data_ptr+inBlock.size(), outBlock.begin());
}

/**
 * Reasonably efficient function that loads a signal from a WAV file (inWav)
 * passes the signal in blocks through a neural network (model) then writes the result
 * out to a new wav file (outWav)
*/
void processSignal(std::string inWav, std::string outWav, torch::jit::script::Module& model){
  std::vector<float> signal = myk_tiny::loadWav(inWav);
  int blockSize = 1024;
  for (int offset=0;offset<signal.size();offset += blockSize){
    int endSample = (offset + blockSize > signal.size()) ? signal.size() : offset + blockSize;
    std::cout << "processing from " << offset << " to " << endSample << std::endl; 
    // create the input for the block processing function
    std::vector<float> inBlock{signal.begin() + offset, signal.begin() + endSample};
    // process the block, writing back out to the same block
    processBlock(model, inBlock, inBlock, blockSize);
    // copy processed audio to the main signal vector
    std::copy(inBlock.begin(), inBlock.end(),signal.begin() + offset);
  }
  myk_tiny::saveWav(signal, 1, 44100, outWav); 
}



int main() {
  std::string modelFile, inWav, outWav;
  inWav = "../../audio/sine_400_16bit.wav";  
  std::vector<float> signal = myk_tiny::loadWav(inWav);

  torch::jit::script::Module my_lstm = torch::jit::load("../python/dist_32.pt");
  
  processSignal(inWav, "proc.wav", my_lstm);
  
  std::cout << "ok\n";
}