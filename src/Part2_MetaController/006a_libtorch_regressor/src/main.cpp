// some ideas from https://github.com/pytorch/examples/blob/main/cpp/regression/regression.cpp
// 

#include <iostream>
#include <torch/torch.h>
#include <vector>
#include <random>


/** get a vector of count values from start to end */
std::vector<float> getRange(float start, float end, int count){
    std::vector<float> data{};
    float d{(end - start) / (count-1)}; // 
    data.resize(count);
    float v{start}; 
    for (int i=0; i<count; ++i){
        data[i] = v;
        v+= d; 
    }
    return data; 
}

void testGetRange1(){
    float start{0}, end{1};
    std::vector<float> d;

    d = getRange(start, end, 10);
    assert(d[0] == start);
    assert(d[d.size() - 1] == end);
}

/** adds noise to the sent data by adding values in the range low to high to each value*/
void addNoise(std::vector<float>& data, float low, float high){
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(low, high);
    auto rand = std::bind ( distribution, generator );
    for (int i=0;i<data.size();++i){
        data[i] += rand();
    }
}

void printData(std::vector<float>& data){
    printf("\n");
    for (float& d : data){
        printf(" %f", d);
    }
}

/** train the sent network on the sent data for one round 
 * i.e. feed inputs, calculate error and gradients from outputs
 * and back prop the error
*/
float runEpoch(torch::nn::Linear& net, torch::optim::SGD& opt, torch::Tensor in_t,  torch::Tensor out_t){

    opt.zero_grad();
    auto output = torch::mse_loss(net(in_t), out_t);
    
    //auto output = torch::smooth_l1_loss(net(in_t), out_t);
    float loss = output.item<float>();
    // Backward pass
    output.backward();
    // Apply gradients
    opt.step();
    return loss; 
}

int main()
{
    //testGetRange1();
    // simple dataset
    int data_size = 100;
    std::vector<float> inputs_x = getRange(0, 1, data_size);
    std::vector<float> outputs_y = getRange(10, 100, data_size);
    printData(outputs_y);
    
    addNoise(outputs_y, -1, 1);
    printData(outputs_y);
    
    //
    // simplest linear regressor
    auto net = torch::nn::Linear(1, 1);
    torch::optim::SGD optimizer(net->parameters(), /*lr=*/0.01);

    // convert data to tensors..
    torch::Tensor in_t = torch::empty({(long) inputs_x.size(), 1});
    torch::Tensor out_t = torch::empty({(long) outputs_y.size(), 1});
    // probably there is a better way to copy
    // the floats into the tensors, but this'll do for now
    // note that we do it from floats 
    // as that is what we'll be doing with the parameters
    for (int i=0; i<inputs_x.size(); ++i){
        in_t[i][0] = inputs_x[i];
    }
    for (int i=0; i<outputs_y.size(); ++i){
        out_t[i] = outputs_y[i];
    }
    // now run some epochs
    float loss {1000};
    int i = 0;
    // we will never have zero loss as 
    // the data is not actually on a straight line
    // due to the random noise we added to it 
    while (loss > 0.1 && i < 5000) { 
    //for (int i=0;i<10000;++i){
        loss = runEpoch(net, optimizer, in_t, out_t);
        
        if (i % 100 == 0) {
            //std::cout << "Params: " << net->parameters() << std::endl;
            printf("%i loss %f \n", i, loss);
        }
        ++i;
    }
    return 0;
}


