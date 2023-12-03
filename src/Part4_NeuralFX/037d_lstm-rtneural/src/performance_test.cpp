#include <iostream>
#include <filesystem>
#include <RTNeural/RTNeural.h>
#include <torch/script.h> // One-stop header.
#include <chrono>

using RTLSTMModel1 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 1>, RTNeural::DenseT<float, 1, 1>>;
using RTLSTMModel2 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 2>, RTNeural::DenseT<float, 2, 1>>;
using RTLSTMModel4 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 4>, RTNeural::DenseT<float, 4, 1>>;
using RTLSTMModel8 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 8>, RTNeural::DenseT<float, 8, 1>>;
using RTLSTMModel16 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 16>, RTNeural::DenseT<float, 16, 1>>;
using RTLSTMModel32 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 32>, RTNeural::DenseT<float, 32, 1>>;
using RTLSTMModel64 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 64>, RTNeural::DenseT<float, 64, 1>>;
using RTLSTMModel128 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 128>, RTNeural::DenseT<float, 128, 1>>;
using RTLSTMModel256 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 256>, RTNeural::DenseT<float, 256, 1>>;
using RTLSTMModel512 = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, 512>, RTNeural::DenseT<float, 512, 1>>;

using RTLSTMModel128L = RTNeural::ModelT<float, 1, 128, RTNeural::LSTMLayerT<float, 1, 128>>;

/** run the sent block of audio through the sent torchscript model, writing the results to send outputs vector */
void processBlockTS(  torch::jit::script::Module& model, std::vector<float>& inBlock, std::vector<float>& outBlock, int numSamples){

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


/** run the sent block of audio through the sent rtneural model, writing results to sent outputs*/
void processBlockRTL(RTLSTMModel128L& model, std::vector<float>& inputs, std::vector<float>& outputs)
{  
    for(size_t i = 0; i < inputs.size(); ++i)
    {
        model.forward(&inputs[i]);
    }
}

/** run the sent block of audio through the sent rtneural model, writing results to sent outputs*/
void processBlockRT(RTLSTMModel128& model, std::vector<float>& inputs, std::vector<float>& outputs)
{  
    for(size_t i = 0; i < inputs.size(); ++i)
    {
        outputs[i] = model.forward(&inputs[i]);
    }
}


int main(int argc, char* argv[])
{
    // setup an input 
    std::vector<float> input{};
    input.resize(44100);
    for (auto i=0;i<input.size();++i){
        input[i] = ((float) i) / input.size();
    }
    std::vector<float> output{};
    output.resize(input.size());
    int size = 128;
    RTLSTMModel128L rtModel;
    
    auto rtModelFilePath = "../models/test_" + std::to_string(size)+ ".json";
    std::ifstream jsonStream(rtModelFilePath, std::ifstream::binary);

    nlohmann::json modelJson;
    jsonStream >> modelJson;

    auto& lstm = rtModel.get<0>();
    RTNeural::torch_helpers::loadLSTM<float> (modelJson, "lstm.", lstm);
    // auto& dense = rtModel.get<1>();
    // RTNeural::torch_helpers::loadDense<float> (modelJson, "dense.", dense);


    rtModel.reset();

    double totalMs{0};
    double runs = 10;
    std::cout << "doing the run " << std::endl;
    for (auto i=0;i<runs; ++i){
        // std::cout << i << " " ; 
      auto startTime = std::chrono::high_resolution_clock::now();
    
      processBlockRTL(rtModel, input, output);

      auto endTime = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
      totalMs += duration.count();

    }
    double meanDur = totalMs / runs; 
    std::cout << size << "," << meanDur << std::endl;
}
