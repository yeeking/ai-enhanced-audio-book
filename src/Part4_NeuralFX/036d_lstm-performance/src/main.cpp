#include <torch/script.h> // One-stop header.
#include "../../tinywav/myk_tiny.h"

#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <filesystem>
#include <chrono>


/*does data processing etc. to send the sent vector through the sent model
*/
void runSignalThoughModel(torch::jit::script::Module& model, std::vector<float>& signal){
    torch::Tensor in_t = torch::from_blob(signal.data(), {static_cast<int64_t>(signal.size())});
    // std::cout << "reshaping tensor" << std::endl;
    in_t = in_t.view({-1, 1});  
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(in_t);

    // std::cout << "Passing through network" << std::endl;
    // https://pytorch.org/cppdocs/api/typedef_namespacetorch_1abf2c764801b507b6a105664a2406a410.html#typedef-torch-nogradguard
    // disables gradient calculation which is not required 
    // for inference, apparently more memory efficient
    // but I do not see a speed difference in this test at least
    torch::NoGradGuard nograd;
    torch::jit::IValue out_ival = model.forward(inputs);

    // std::cout << "Converting result to vector" << std::endl;
    auto out_elements = out_ival.toTuple()->elements();
    torch::Tensor out_t = out_elements[0].toTensor();
    out_t = out_t.view({-1});
    float* data_ptr = out_t.data_ptr<float>();
    std::vector<float> data_vector(data_ptr, data_ptr + out_t.numel());

}

int main() {
  torch::jit::script::Module my_lstm;
  std::string modelFile, inWav, outWav;
  // inWav = "../../audio/drums_16bit.wav";
  inWav = "../../audio/sine_400_16bit.wav";  
  outWav = "test.wav";
  std::cout << "Loading WAV " << inWav << std::endl;    
  std::vector<float> signal = myk_tiny::loadWav(inWav);
 
  // Deserialize the ScriptModule from a file using torch::jit::load().

  for (int pow =0;pow < 10; ++pow){
    int val =  std::pow(2.0, pow);
    modelFile = "../my_lstm_" + std::to_string(val) + ".pt";
    if (!std::filesystem::exists(modelFile)) {
      std::cout << "Error: cannot find model file " << modelFile << " did you run the python script to generate the models? " << std::endl;
      break;
    } 
    // std::cout << "Loading model " << modelFile << std::endl;    
    my_lstm = torch::jit::load(modelFile);
    // do a warm up run 
    runSignalThoughModel(my_lstm, signal);
    // start the timer 

    double totalMs{0};
    double runs = 10;
//    std::cout << "Timing " << runs << " runs nodes: " << val << std::endl;
    for (auto i=0;i<runs; ++i){
      auto startTime = std::chrono::high_resolution_clock::now();
    
      runSignalThoughModel(my_lstm, signal);
    
      auto endTime = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
      totalMs += duration.count();

    }
    double meanDur = totalMs / runs; 
  //  std::cout << "Mean Inference + data proc for " << signal.size() << " samples and " << val << " LSTM nodes: " << meanDur << " milliseconds" << std::endl;
  std::cout << val << "," << meanDur << std::endl;
  }
  

  std::cout << "ok\n";
}