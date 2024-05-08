#include <torch/torch.h>

class NeuralNetworkLinear : torch::nn::Module {
 public:
 /**
  * @brief Construct a neural network with the sent number of ins and outs
  * 
  */
    NeuralNetworkLinear(int64_t n_inputs, int64_t n_outputs){
        linear = register_module("linear", torch::nn::Linear(n_inputs, n_outputs));
    }
    /**
     * @brief pass the sent vector of inputs through the neural network and return the outputs
     * 
     * @return std::vector<float> 
     */
    std::vector<float> forward(const std::vector<float>& input){
        std::cout << "nn::forward in " << input << std::endl;

        torch::Tensor inT = torch::from_blob((float*)(input.data()), input.size());
        // call the 
        torch::Tensor outT = this->forward(inT);
        // re-arrange the data in memory so it is contiguous
        outT = outT.contiguous();
        // whack it into a vector 
        std::vector<float> output(outT.data_ptr<float>(), outT.data_ptr<float>()+outT.numel());
        std::cout << "nn::forward out " << output << std::endl;

        return output; 
    }
 private:
    torch::nn::Linear linear{nullptr};


    
    torch::Tensor forward(torch::Tensor input){
        input = linear(input);        
        return input;
    }


};       

class NeuralNetwork : torch::nn::Module {
 public:
 /**
  * @brief Construct a neural network with the sent number of ins and outs
  * 
  */
    NeuralNetwork(int64_t n_inputs, int64_t n_outputs){
        linear = register_module("linear", torch::nn::Linear(n_inputs, n_outputs));
        sigmoid = register_module("sigmoid", torch::nn::Sigmoid());
        softmax = register_module("softmax", torch::nn::Softmax(0));


    }
    /**
     * @brief pass the sent vector of inputs through the neural network and return the outputs
     * 
     * @return std::vector<float> 
     */
    std::vector<float> forward(const std::vector<float>& input){
        std::cout << "nn::forward in " << input << std::endl;

        torch::Tensor inT = torch::from_blob((float*)(input.data()), input.size());
        // call the 
        torch::Tensor outT = this->forward(inT);
        // re-arrange the data in memory so it is contiguous
        outT = outT.contiguous();
        // whack it into a vector 
        std::vector<float> output(outT.data_ptr<float>(), outT.data_ptr<float>()+outT.numel());
        std::cout << "nn::forward out " << output << std::endl;

        return output; 
    }
 private:
    torch::nn::Linear linear{nullptr};
    torch::nn::Sigmoid sigmoid{nullptr};
    torch::nn::Softmax softmax{nullptr};

    torch::Tensor forward(torch::Tensor input){
        input = linear(input);  
        input = sigmoid(input);  
        
        input = softmax(input); // sum(input) is now 1      
        return input;
    }


};       