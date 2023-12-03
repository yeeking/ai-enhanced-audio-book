#include "NeuralNetwork.h"
#include <iostream> 

int main()
{
    NeuralNetwork nn{1, 2};
    for (float i=0;i<10; ++i){
        nn.addTrainingData({i/10}, {i/5, i/3});
    }
    nn.runTraining(100000);
    
    return 0; 
}