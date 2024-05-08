#include "NeuralNetwork.h"
#include <iostream>

NeuralNetwork::NeuralNetwork(int64_t _n_inputs, int64_t _n_outputs)
: n_inputs{_n_inputs}, n_outputs{_n_outputs}
{
  linear = register_module(
                "linear", 
                torch::nn::Linear(n_inputs, n_outputs)
            );
  softmax = register_module(
                "softmax", 
                torch::nn::Softmax(1)
  );
}

std::vector<float> NeuralNetwork::forward(const std::vector<float>& inputs)
{
    // copy input data into a tensor 
    torch::Tensor in_t = torch::empty({1, n_inputs});
    for (long i=0; i<n_inputs; ++i){
       in_t[0][i] = inputs[i];    
    }
    // pass through the network:
    torch::Tensor out_t = forward(in_t);
    // copy output back out to a vector 
    std::vector<float> outputs(n_outputs);// initialise size to n_outputs
    for (long i=0; i<n_outputs; ++i){
       outputs[i] = out_t[0][i].item<float>();
    }
    return outputs;    
}

void NeuralNetwork::addTrainingData(
             std::vector<float>inputs, 
             std::vector<float>outputs)
{

}

void NeuralNetwork::runTraining(int epochs)
{

}

torch::Tensor NeuralNetwork::forward(const torch::Tensor& input)
{
    std::cout << "forward input " << input << std::endl;
    torch::Tensor out = linear(input);
    std::cout << "forward after linear " << out << std::endl;
    out = softmax(out);
    std::cout << "forward after softmax " << out << std::endl;
    return out; 
}