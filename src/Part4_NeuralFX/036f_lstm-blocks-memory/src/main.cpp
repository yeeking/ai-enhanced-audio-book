#include <torch/script.h> // One-stop header.
#include "../../tinywav/myk_tiny.h"

#include <iostream>
#include <memory>
#include <vector>

// the data type for the LSTM state has a long name
// so we'll use a typedef to make a custom name
typedef c10::intrusive_ptr<c10::ivalue::Tuple> LSTMState;

void processBlock(  torch::jit::script::Module& model, std::vector<float>& inBlock, std::vector<float>& outBlock, int numSamples){

  torch::Tensor in_t = torch::from_blob(inBlock.data(), {static_cast<int64_t>(numSamples)});
  in_t = in_t.view({-1, 1});  
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(in_t);
  torch::jit::IValue out_ival = model.forward(inputs);
  // copy to the outBlock
  auto out_elements = out_ival.toTuple()->elements();
  torch::Tensor out_t = out_elements[0].toTensor();
  LSTMState state = out_elements[1].toTuple();
  
  out_t = out_t.view({-1});
  float* data_ptr = out_t.data_ptr<float>();
  for (auto i=0;i<numSamples;++i){
    outBlock[i] = data_ptr[i];
  }
}

LSTMState processBlockState(  torch::jit::script::Module& model, const LSTMState& state, std::vector<float>& inBlock, std::vector<float>& outBlock, int numSamples){

  torch::Tensor in_t = torch::from_blob(inBlock.data(), {static_cast<int64_t>(numSamples)});
  in_t = in_t.view({-1, 1});  
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(in_t);
  inputs.push_back(state);
  
  torch::jit::IValue out_ival = model.forward(inputs);
  // copy to the outBlock
  auto out_elements = out_ival.toTuple()->elements();
  torch::Tensor out_t = out_elements[0].toTensor();
  out_t = out_t.view({-1});
  float* data_ptr = out_t.data_ptr<float>();
  std::copy(data_ptr, data_ptr+inBlock.size(), outBlock.begin());
  // now retain the state
  return out_elements[1].toTuple();
}
LSTMState getRandomStartState(int numLayers, int hiddenSize){
    torch::Tensor h0 = torch::randn({numLayers, hiddenSize});
    torch::Tensor c0 = torch::randn({numLayers, hiddenSize});
    LSTMState state = c10::ivalue::Tuple::create({h0, c0}); 
    return state;
}

void processSignalState(std::string inWav, std::string outWav, torch::jit::script::Module& model){
  std::vector<float> signal = myk_tiny::loadWav(inWav);
  
  LSTMState state = getRandomStartState(1, 1);
  int blockSize = 1024;
  std::vector<float> outBlock(blockSize);

  for (int offset=0;offset<signal.size();offset += blockSize){
    int endSample = (offset + blockSize > signal.size()) ? signal.size() : offset + blockSize;
    std::cout << "processing from " << offset << " to " << endSample << std::endl; 
    
    std::vector<float> inBlock{signal.begin() + offset, signal.begin() + endSample};
    // state = processBlockState(model, state, inBlock, inBlock, inBlock.size());
    state = processBlockState(model, state, inBlock, outBlock, inBlock.size());
    // copy result back out to signal array
    std::copy(outBlock.begin(), outBlock.begin() + inBlock.size(), signal.begin() + offset);

  }
  myk_tiny::saveWav(signal, 1, 44100, outWav); 
}


void processSignalNoState(std::string inWav, std::string outWav, torch::jit::script::Module& model){
  std::vector<float> signal = myk_tiny::loadWav(inWav);
  int blockSize = 100;
  for (int offset=0;offset<signal.size();offset += blockSize){
    int endSample = (offset + blockSize > signal.size()) ? signal.size() : offset + blockSize;
    std::cout << "processing from " << offset << " to " << endSample << std::endl; 
    
    std::vector<float> inBlock{signal.begin() + offset, signal.begin() + endSample};
    processBlock(model, inBlock, inBlock, blockSize);
    // copy result back out to signal array
    for (int i=offset; i<endSample;++i){
      signal[i] = inBlock[i-offset];
    }
  }
  myk_tiny::saveWav(signal, 1, 44100, outWav); 
}



int main() {
  std::string modelFile, inWav, outWav;
  inWav = "../../audio/sine_400_16bit.wav";  
  std::vector<float> signal = myk_tiny::loadWav(inWav);

  torch::jit::script::Module my_lstm = torch::jit::load("../my_lstm_no_state.pt");
  torch::jit::script::Module my_lstm_state = torch::jit::load("../my_lstm_with_state.pt");
  
  processSignalNoState(inWav, "proc_no_state.wav", my_lstm);
  processSignalState(inWav, "proc_with_state.wav", my_lstm_state);
  
  std::cout << "ok\n";
}