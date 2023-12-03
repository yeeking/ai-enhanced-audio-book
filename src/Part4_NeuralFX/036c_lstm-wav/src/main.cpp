#include <torch/script.h> // One-stop header.
#include "../../tinywav/myk_tiny.h"

#include <iostream>
#include <memory>
#include <vector>


int main() {
  torch::jit::script::Module my_lstm;
  std::string modelFile, inWav, outWav;
  modelFile = "../my_lstm.pt";
  // inWav = "../../audio/drums_16bit.wav";
  inWav = "../../audio/sine_400_16bit.wav";  
  outWav = "test.wav";

  // Deserialize the ScriptModule from a file using torch::jit::load().
  std::cout << "Loading model " << modelFile << std::endl;    
  my_lstm = torch::jit::load(modelFile);
  std::cout << "Loading WAV " << inWav << std::endl;    
  std::vector<float> signal = myk_tiny::loadWav(inWav);
  std::cout << "Read " << signal.size() << " samples " << std::endl;
  std::cout << "converting to tensor" << std::endl;
  torch::Tensor in_t = torch::from_blob(signal.data(), {static_cast<int64_t>(signal.size())});

  std::cout << "reshaping tensor" << std::endl;
  in_t = in_t.view({-1, 1});  
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(in_t);

  std::cout << "Passing through network" << std::endl;
  torch::jit::IValue out_ival = my_lstm.forward(inputs);

  std::cout << "Converting result to vector" << std::endl;
  auto out_elements = out_ival.toTuple()->elements();
  torch::Tensor out_t = out_elements[0].toTensor();
  out_t = out_t.view({-1});
  float* data_ptr = out_t.data_ptr<float>();
  std::vector<float> data_vector(data_ptr, data_ptr + out_t.numel());

  std::cout << "Saving WAV " << outWav << std::endl;
  myk_tiny::saveWav(data_vector, 1, 44100, outWav); 



  std::cout << "ok\n";
}