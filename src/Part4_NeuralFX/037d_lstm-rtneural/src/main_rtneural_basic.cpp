#include <iostream>
#include <filesystem>
#include <RTNeural/RTNeural.h>

// this is the definition of the actual 
// model architecture, 
// in this case 1->32->1 lstm->dense (aka linear in torch)
const int lstm_units = 64;
using ModelType = RTNeural::ModelT<float, 1, 1, RTNeural::LSTMLayerT<float, 1, lstm_units>, RTNeural::DenseT<float, lstm_units, 1>>;

void loadModel(std::ifstream& jsonStream, ModelType& model)
{
    nlohmann::json modelJson;
    jsonStream >> modelJson;

    auto& lstm = model.get<0>();
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
  
    auto& dense = model.get<1>();
    // as per the lstm prefix, here the json needs a key prefixed with dense. 
    RTNeural::torch_helpers::loadDense<float> (modelJson, "dense.", dense);
}


int main(int argc, char* argv[])
{
    auto modelFilePath = "../models/lstm_64.json";
    assert(std::filesystem::exists(modelFilePath));

    std::cout << "Loading model from path: " << modelFilePath << std::endl;
    
    std::ifstream jsonStream(modelFilePath, std::ifstream::binary);
    ModelType model;
    loadModel(jsonStream, model);
    model.reset();

    // now take the model for a spin :) 
    std::vector<float> inputs {1.0, 2.0, 3.0, 4.0};
    std::vector<float> outputs {};
    outputs.resize(inputs.size(), {});

    for(size_t i = 0; i < inputs.size(); ++i)
    {
        outputs[i] = model.forward(&inputs[i]);
        std::cout << "in " << inputs[i] << " out: " << outputs[i] << std::endl;
    }
}
