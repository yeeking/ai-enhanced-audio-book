#include <vector>
#include <iostream>
#include <random>
#include <torch/torch.h>

int main() {
    auto net = torch::nn::Linear(2, 3) ;  
    for (const auto& p : net->parameters()) {
        std::cout << p << std::endl;
    }
    auto input = torch::empty({1, 2});
    input[0][0] = 0.5;
    input[0][1] = 0.25;
    std::cout << "Passing 0.5 in ... this came out:\n" 
    << net(input) << std::endl;
}   