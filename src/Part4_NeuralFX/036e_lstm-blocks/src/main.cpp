#include <torch/script.h> // One-stop header.
#include "../../tinywav/myk_tiny.h"

#include <iostream>
#include <memory>
#include <vector>

/** pass inBlock through sent model and write results to outBlock*/
void processBlock(torch::jit::script::Module& model, 
                  std::vector<float>& inBlock, 
                  std::vector<float>& outBlock){
    
  torch::Tensor in_t = torch::from_blob(inBlock.data(), {static_cast<int64_t>(inBlock.size())});

  in_t = in_t.view({-1, 1});  
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(in_t);

  torch::jit::IValue out_ival = model.forward(inputs);

  auto out_elements = out_ival.toTuple()->elements();
  torch::Tensor out_t = out_elements[0].toTensor();
  out_t = out_t.view({-1});
  float* data_ptr = out_t.data_ptr<float>();
  std::vector<float> data_vector(data_ptr, data_ptr + out_t.numel());
  if(data_vector.size() == outBlock.size())
    {std::copy(data_vector.begin(), data_vector.end(), outBlock.begin());}
}

int main() {
  // torch::jit::script::Module my_lstm;
  // std::string modelFile, inWav, outWav;
  // modelFile = "../my_lstm.pt";
  // // inWav = "../../audio/drums_16bit.wav";
  // inWav = "../../audio/sine_400_16bit.wav";  
  // outWav = "test.wav";

  // // Deserialize the ScriptModule from a file using torch::jit::load().
  // std::cout << "Loading model " << modelFile << std::endl;    
  // my_lstm = torch::jit::load(modelFile);
  // std::cout << "Loading WAV " << inWav << std::endl;    
  // std::vector<float> signal = myk_tiny::loadWav(inWav);
  // std::cout << "Read " << signal.size() << " samples " << std::endl;
  
  // int blockSize = 1024;
  // std::vector<float> outSignal(signal.size());
  // std::vector<float> inBlock(blockSize);
  // std::vector<float> outBlock(blockSize);

  // for (auto start=0;start + blockSize < signal.size(); start += blockSize){
  //   std::cout << "from " << start << " to " << (start + blockSize) << std::endl;
  //   std::copy(signal.begin() + start, 
  //           signal.begin() + start + blockSize, inBlock.begin());
  //   processBlock(my_lstm, inBlock, outBlock);
  //   // copy to outSignal - won't need to do that in realtime situation
  //   std::copy(outBlock.begin(), outBlock.end(), outSignal.begin() + start);
  // }


  // std::cout << "Saving WAV " << outWav << std::endl;
  // myk_tiny::saveWav(outSignal, 1, 44100, outWav); 

// load model 
torch::jit::script::Module  my_lstm = torch::jit::load("../my_lstm.pt");
// load WAV 
std::vector<float> signal = myk_tiny::loadWav("../../audio/sine_400_16bit.wav");
// setup vector to store processed signal
std::vector<float> outSignal(signal.size());

// setup blocks for processing
int blockSize = 1024;
std::vector<float> inBlock(blockSize);
std::vector<float> outBlock(blockSize);
// loop through jumping a block at a time
for (auto s=0;s + blockSize < signal.size(); s += blockSize){
    // copy signal into inBlock
    std::copy(signal.begin() + s, 
              signal.begin() + s + blockSize, 
              inBlock.begin());
    processBlock(my_lstm, inBlock, outBlock);
    // copy to outBlock to outSignal
    // (won't need to do that in realtime situation)
    std::copy(outBlock.begin(), outBlock.end(), outSignal.begin() + s);
}
// save to WAV
  myk_tiny::saveWav(outSignal, 1, 44100, "test.wav"); 

  std::cout << "ok\n";
}