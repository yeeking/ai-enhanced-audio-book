#include <torch/script.h> // One-stop header.

#include <iostream>
#include <memory>

int main() {
  torch::jit::script::Module my_lstm;
  try {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    my_lstm = torch::jit::load("../my_lstm.pt");
    torch::Tensor in_t = torch::zeros({10, 1});
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(in_t);
    in_t[0][0] = 1.0;
    torch::jit::IValue out_t = my_lstm.forward(inputs);
    std::cout << out_t << std::endl;
  }
  catch (const c10::Error& e) {
    std::cerr << "error loading the model\n";
    return -1;
  }

  std::cout << "ok\n";
}