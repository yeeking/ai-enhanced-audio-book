#include <iostream>
#include <filesystem>
#include <RTNeural/RTNeural.h>
#include <torch/script.h> // One-stop header.

// this is the definition of the actual 
// model architecture, 
// in this case 1->32->1 lstm->dense (aka linear in torch)
const int lstm_units = 64;
using RTLSTMModel = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, lstm_units>, RTNeural::DenseT<float, lstm_units, 1>>;

/** Create a RTLSTMModel model and load weights into it from json file at jsonPath
 * returns the model 
*/
RTLSTMModel getRTLTSMModel(std::string jsonPath)
{   
    std::cout << "Loading model from path: " << jsonPath << std::endl; 


    std::ifstream jsonStream(jsonPath, std::ifstream::binary);
    RTLSTMModel rtModel;

    nlohmann::json modelJson;
    jsonStream >> modelJson;

    auto& lstm = rtModel.get<0>();
    // hacky = assumes one layer
    const int json_lstm_size = modelJson["lstm.weight_ih_l0"].size()/4;
    if (json_lstm_size != lstm_units){
        std::cout << "Model wants " << lstm_units << " lstm units but JSON file specifies " << json_lstm_size << std::endl;
        throw(std::exception());
    }
    std::cout << modelJson["lstm.weight_ih_l0"].size()/4 << std::endl;
    // note that the "lstm." is a prefix used to find the 
    // lstm data in the json file so your python
    // needs to name the lstm layer 'lstm' if you use lstm. as your prefix
    RTNeural::torch_helpers::loadLSTM<float> (modelJson, "lstm.", lstm);
  
    auto& dense = rtModel.get<1>();
    // as per the lstm prefix, here the json needs a key prefixed with dense. 
    RTNeural::torch_helpers::loadDense<float> (modelJson, "dense.", dense);

    // loadModel(jsonStream, rtModel);
    rtModel.reset();
  
    return rtModel; 
}

/** run the sent block of audio through the sent rtneural model, writing results to sent outputs*/
void processBlockRT(RTLSTMModel& model, std::vector<float>& inputs, std::vector<float>& outputs)
{  
    for(size_t i = 0; i < inputs.size(); ++i)
    {
        outputs[i] = model.forward(&inputs[i]);
    }
}

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



int main(int argc, char* argv[])
{
    auto rtModelFilePath = "../models/lstm_64.json";//"../dist_32.json";
    auto tsModelFilePath = "../models/lstm_64.ts"; //"../dist_32.ts";

    // load rtneural model
    RTLSTMModel rtModel = getRTLTSMModel(rtModelFilePath);  
    // load torchscript model
    torch::jit::script::Module tsModel = torch::jit::load(tsModelFilePath);
  
    std::vector<float> inputs {1.0, 2.0, 3.0, 4.0};
    std::vector<float> outputsRT{};
    std::vector<float> outputsTS{};
    outputsRT.resize(inputs.size(), {});
    outputsTS.resize(inputs.size(), {});
   
    processBlockRT(rtModel, inputs, outputsRT);
    processBlockTS(tsModel, inputs, outputsTS, inputs.size());

    for (auto i=0;i<inputs.size(); ++i){
        printf("RT: %f TS: %f \n", outputsRT[i], outputsTS[i]);
    } 
}
