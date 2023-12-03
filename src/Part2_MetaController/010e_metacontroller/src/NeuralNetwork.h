#pragma once

#include <torch/torch.h>

class NeuralNetwork : torch::nn::Module {
    public:
        NeuralNetwork(int64_t n_inputs, int64_t n_outputs);
        std::vector<float> forward(const std::vector<float>& inputs);
        void addTrainingData(
             std::vector<float>inputs, 
             std::vector<float>outputs);
        void runTraining(int epochs);
    private:
        int64_t n_inputs;
        int64_t n_outputs;
        std::vector<torch::Tensor> trainInputs;
        std::vector<torch::Tensor> trainOutputs;
        // unique ptr so we can initialise it 
        // in the constructor after building the model
        std::unique_ptr<torch::optim::SGD> optimiser;
        torch::nn::Linear linear1{nullptr};
        torch::nn::Sigmoid sig1{nullptr};
        torch::nn::Linear linear2{nullptr};
        torch::nn::Softmax softmax{nullptr};
        torch::Tensor forward(const torch::Tensor& input);
}; 