#include <vector>
#include <iostream>
#include <random>
#include <torch/torch.h>

class NeuralNetwork : torch::nn::Module {
    public:
        NeuralNetwork(int64_t n_inputs, int64_t n_outputs){
            linear = register_module("linear", torch::nn::Linear(n_inputs, n_inputs));
            relu = register_module("relu", torch::nn::ReLU(n_inputs));

        }  
        /** pass the sent input through the network 
         * and return the output
        */
        torch::Tensor forward(torch::Tensor input){
            // pass input through linear layer
            std::cout << "forward: input  : " << input << std::endl;
            input = linear(input);
            std::cout << "linear: output  : " << input << std::endl;
            input = relu(input);
            std::cout << "relu: output  : " << input << std::endl;
            return input; 
        }      
    private:
        // https://pytorch.org/tutorials/advanced/cpp_frontend.html
        // there are different ways to manage the creation 
        // of the model. 
        // I prefer this nullptr style to the 'initialiser list' style
        // as the model building code goes in the constructor which
        // is more readable

        torch::nn::Linear linear{nullptr}; 
        torch::nn::ReLU relu{nullptr};
        
};        


/**
 * @brief Generate x,y values on a linear ramp. y = m(x) + b 
 */
std::vector<std::pair<float,float>>  getLine(float b, float m, 
                          float startX, float endX, float count)

{
    float y{0}, dX{0};
    std::vector<std::pair<float,float>> xys; 
    dX = (endX - startX) / count; // change in x
    for (float x = startX; x < endX; x+=dX){
        y = (m * x) + b;
        xys.push_back({x,y});
    }
    return xys; 
}

void printXYs(std::vector<std::pair<float,float>>& xys){
    for (int i=0;i<xys.size();++i){
        std::cout << "x: " << xys[i].first << ", y: " << xys[i].second << std::endl;
    }
}

/**
 * @brief 
 * 
 */
void addNoiseToYValues(std::vector<std::pair<float,float>>& xys, 
                       float low, float high)
{
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(low, high);
    auto rand = std::bind ( distribution, generator );
    for (int i=0;i<xys.size();++i){
        float r = rand();
        std::cout << "rand: " << r << std::endl;
        xys[i].second += r;
    }
}



int main()
{
    std::vector<std::pair<float,float>> xys;
    xys = getLine(5.0, 0.5, 0.0, 10.0, 5);
    printXYs(xys);
    addNoiseToYValues(xys, -0.1, 0.1);
    printXYs(xys);

    NeuralNetwork net{1, 1};

    // for (auto p : net->parameters()){
    //     std::cout << "parameter: " << p << std::endl;
    // }

    torch::Tensor in_t = torch::empty({(long) xys.size(), 1});

    for (int i=0; i<xys.size(); ++i){
        in_t[i][0] = xys[i].first; // first is x    
    }

    torch::Tensor out_t = net.forward(in_t);

    std::cout << "linear and relu output : " << out_t << std::endl;

    
}