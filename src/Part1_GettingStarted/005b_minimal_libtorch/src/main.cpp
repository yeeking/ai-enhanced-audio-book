#include <torch/torch.h>
#include <iostream>

int main() {
  torch::Tensor t = torch::rand({2, 2});
  std::cout << t << std::endl;
}
