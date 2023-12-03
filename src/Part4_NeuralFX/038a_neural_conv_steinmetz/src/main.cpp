#include <torch/script.h> // One-stop header.
#include <torch/torch.h>
#include <iostream>
#include <memory>
#include <string>
#include "tinywav_help.h"
//#include "tinywav/tinywav.h"

/** generates a random input for the neural network and does inference*/
void randomInfer(torch::jit::script::Module& module){
  torch::IntArrayRef xshape = {1, 1, 100000};
    torch::IntArrayRef cshape = {1, 1, 2};

    torch::jit::IValue x(torch::rand(xshape));// audio
    torch::jit::IValue c(torch::rand(cshape));// conditioning
    
    std::vector<torch::jit::IValue> inputs{};
    inputs.push_back(x);

    torch::jit::Kwargs extra_args;
    extra_args["c"] = c;
    // IValue forward(std::vector<IValue> inputs, const Kwargs& kwargs = Kwargs()) {
    auto outputs = module.forward(inputs, extra_args);
    std::cout << outputs << std::endl;

}

void audioInfer(torch::jit::script::Module& module, std::string infile, std::string outfile){
  std::vector<float> audio = tinywav_loadWav(infile);
  signed long in_size = (88889 * 2) + audio.size();

  torch::Tensor in_t = torch::empty({1, 1, in_size});
  // front pad with silence
  long i=0;
  for (; i<88889; ++i){
      in_t[0][0][i] = 0;  
  }  
  for (; (i-88889)<audio.size(); ++i){
       in_t[0][0][i] = audio[i-88889];    
  }
  for (; i<in_size; ++i){
      in_t[0][0][i] = 0;  
  }
  
  std::vector<torch::jit::IValue> inputs{};
  inputs.push_back(in_t);
  torch::IntArrayRef cshape = {1, 1, 2};
  //torch::jit::IValue c(torch::rand(cshape));// conditioning
  torch::jit::IValue c(torch::tensor({{{0.5, -0.5}}}));// conditioning
  torch::jit::Kwargs extra_args;
  extra_args["c"] = c;
  std::cout << "Feeding in audio shape " << inputs[0].toTensor().sizes() << std::endl;
  torch::jit::IValue outputs = module.forward(inputs, extra_args);

  torch::Tensor tout = outputs.toTensor();
  std::cout << "Received output of shape " << tout.sizes() << std::endl; 
  std::vector<float> fs(tout.sizes()[2], 0.0f);
  float max = torch::max(torch::abs(tout)).item<float>();
  std::cout << "max " << max << std::endl; 
  for (auto i=0;i<fs.size();i++){
    fs[i] = tout[0][0][i].item<float>();
    fs[i] /= max; 
  }
  tinywav_myk_tiny::saveWav(fs, 1, 44100, outfile);  
}

void loadVecFloatToVecIVal(std::vector<float>& floats, 
                           std::vector<torch::jit::IValue>& ivv){
  // pull out the tensor so we can put the data into it 
  torch::Tensor& tens = ivv[0].toTensor();

  // Use copy_() to copy data from the float vector into the existing tensor
  tens.copy_(torch::from_blob(floats.data(), {static_cast<long>(floats.size())},
                                           torch::kFloat32));

}

/** in place processing of the set block into the sent out block*/
void processBlock(torch::jit::script::Module& module, 
                        std::vector<float>& inBlock, 
                        std::vector<float>& outBlock, 
                        std::vector<torch::jit::IValue>& inputIValue, 
                        torch::jit::Kwargs& conditioningParams){

  // load the floats into the ivalue
  loadVecFloatToVecIVal(inBlock,inputIValue);  
  // inference
  torch::jit::IValue outputs = module.forward(inputIValue, conditioningParams);
  // convert result back to tensor 
  torch::Tensor& tout = outputs.toTensor();
  // copy tensor into the float vector
  //std::memcpy(outBlock.data(), tout.data_ptr(), tout.numel() * sizeof(float));
  std::cout << "got output of size " << tout.sizes() << std::endl; 
  for (auto i=0;i<tout.sizes()[0];++i){
    std::cout << "out b " << outBlock[i] << std::endl;
    outBlock[i] = tout[0][0][i].item<float>();
    std::cout << "out a " << outBlock[i] << std::endl;

  }
}

void audioInferBlock(torch::jit::script::Module& module, std::string infile, std::string outfile, signed long blockSize){
  std::vector<float> audio_in = tinywav_loadWav(infile);
  std::vector<float> audio_out(audio_in.size(), 0.0f);
  // prepare conditioning params
  torch::IntArrayRef cshape = {1, 1, 2};
  torch::jit::IValue conditioningParams(torch::rand(cshape));// conditioning
  torch::jit::Kwargs extra_args;
  extra_args["c"] = conditioningParams;
  // now pass it in blockwise
  // signed long blockSize = 44100;

  // prepare an input ivalue vector containing a tensor of the correct size
  torch::Tensor in_t = torch::empty({1, 1, blockSize});
  std::vector<torch::jit::IValue> iValVec{};
  iValVec.push_back(in_t);


  for (auto i=0;i+blockSize<audio_in.size();i+=blockSize){
    std::cout << "proc block from " << i << " to "<< (i+blockSize) << std::endl;
    // convert a sub list from audio_in into a vector of ivalues
    auto startIt = audio_in.begin() + i;  // Start at index 2
    auto endIt = audio_in.begin() + i + blockSize;    // End at index 4 (exclusive)
    auto startItOut = audio_out.begin() + i;  // Start at index 2
    auto endItOut = audio_out.begin() + i + blockSize;    // End at index 4 (exclusive)
    
    std::vector<float> inBlock(startIt, endIt);
    std::vector<float> outBlock(startItOut, endItOut);
    
    processBlock(module, 
                inBlock, 
                outBlock, 
                iValVec, 
                extra_args);

  }

  std::cout << "Saving the file" << std::endl;
  tinywav_myk_tiny::saveWav(audio_out, 1, 44100, outfile);  
}


int main(int argc, const char* argv[]) {
  if (argc != 4) {
    std::cerr << "usage: verb-app <path-to-exported-script-module> input wav, output wav\n";
    return -1;
  }

  torch::jit::script::Module module;
  //try {

    // Deserialize the ScriptModule from a file using torch::jit::load().
    module = torch::jit::load(argv[1]);
    //randomInfer(module);

    std::string infile{argv[2]};
    std::string outfile{argv[3]};

     audioInfer(module, infile, outfile);

  // }

  // catch (const c10::Error& e) {
  //   std::cerr << "error loading the model\n";
  //   return -1;
  // }

  std::cout << "ok\n";
}
