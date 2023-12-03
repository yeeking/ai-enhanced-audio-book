#include <vector>
#include <iostream>
#include <random>
#include <torch/torch.h>

class NeuralNetwork : torch::nn::Module {
    public:
        NeuralNetwork(int64_t n_inputs, int64_t n_outputs){
            linear = register_module("linear", torch::nn::Linear(n_inputs, n_inputs));
        }  
        /** pass the sent input through the network 
         * and return the output
        */
        torch::Tensor forward(torch::Tensor input){
            input = linear(input);
            return input; 
        }      
    private:
        torch::nn::Linear linear{nullptr}; 
        
};        


/**
 * @brief Generate x,y values on a linear ramp. y = m(x) + b 
 * 
 * @param b : bias 
 * @param m : multiplier 
 * @param startX: start of value range 
 * @param endX : end of value range
 * @param count : how many x,y pairs to generate
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
 * @param xys
 * @param low: 
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

    auto net = torch::nn::Linear(1, 1);
    for (auto p : net->parameters()){
        std::cout << "parameter: " << p << std::endl;
    }

    torch::Tensor in_t = torch::empty({(long) xys.size(), 1});

    for (int i=0; i<xys.size(); ++i){
        in_t[i][0] = xys[i].first; // first is x    
    }

    torch::Tensor out_t = net(in_t);

    std::cout << "output: " << out_t << std::endl;
    torch::Tensor correct_t;
    correct_t = torch::empty({(long) xys.size(), 1});
    for (int i=0; i<xys.size(); ++i){
        correct_t[i][0] = xys[i].second; // second is y
    } 
    
    
    torch::optim::SGD optimizer(net->parameters(), /*lr=*/0.01);
    
    float loss = 1000;
    while(loss > 0.5){
        torch::Tensor loss_t = torch::mse_loss(net(in_t), correct_t);
        loss = loss_t.item<float>();
        std::cout << "Loss: " << loss << std::endl;
        optimizer.zero_grad();
        loss_t.backward();
        optimizer.step();
    }
    std::cout << net(in_t) << std::endl;
}