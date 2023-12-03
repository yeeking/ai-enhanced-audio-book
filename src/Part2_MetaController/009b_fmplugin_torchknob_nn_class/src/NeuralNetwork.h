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
        torch::nn::Linear linear{nullptr};
        torch::nn::Softmax softmax{nullptr};
        torch::Tensor forward(const torch::Tensor& input);
        
}; 