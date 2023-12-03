#include "NeuralNetwork.h"
#include <iostream> 

int main()
{
    NeuralNetwork nn{1, 2};
    for (float i=0;i<10; ++i){
        std::cout << nn.forward({i/10}) << std::endl;
    }
    return 0; 
}