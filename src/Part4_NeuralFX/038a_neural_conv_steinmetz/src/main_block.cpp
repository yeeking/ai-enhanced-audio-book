#include <torch/script.h> // One-stop header.
#include <torch/torch.h>
#include <iostream>
#include <memory>
#include <string>
#include "tinywav_help.h"
#include <cassert>
#include <chrono>

/// clean implementation of block based processing
/// using Steinmetz' TCN network

// puts the conditioning parameters into the correct structure 
torch::jit::Kwargs getConditioning(float c1, float c2){
  torch::jit::IValue c(torch::tensor({{{c1, c2}}}));// conditioning
  torch::jit::Kwargs extra_args;
  extra_args["c"] = c;
  return extra_args; 
}

void processBlock( torch::jit::script::Module& module,
                        long blockSize, // signal length (len inBlock)
                        long headPadSize, // zero padding to prefix inBlock 
                        long tailPadSize, // zero padding to append inBlock
                        torch::Tensor& inBufferT, // reusable input tensor should be size 1,1,(blockSize+headPad+tailPad)
                        torch::jit::Kwargs& conditioningParams, 
                      //  torch::Tensor& outBufferT, // reusable output tensor should be 
                        std::vector<float>& inBlockV, // signal - copy this into inBufferT after the pad
                        std::vector<float>& outBlockV, // copy signal.length of result here 
                        std::vector<float>& tailBlockV, // copy bit after signal.length of result here after pullingtailPadSize off the start and adding to outBlock
                        std::vector<float>& outputTempV // vector to store complete output from network  
                        
                    ){
    // check sizes of things
    assert (inBufferT.sizes().size() == 3);// 3d input
    assert (inBufferT.sizes()[2] == headPadSize + blockSize + tailPadSize);
    assert (inBlockV.size() == blockSize);
    assert (outBlockV.size() == inBlockV.size());
    assert (tailBlockV.size() == tailPadSize);
    assert (outputTempV.size() == blockSize + tailPadSize);

    //assert (outBufferT.sizes()[2] == blockSize + tailPadSize);
    // prepare the input data
    // zero it all out first
    inBufferT.fill_(0);
    // signal
    for (auto i=headPadSize;i<headPadSize+blockSize;++i){
        inBufferT[0][0][i] = inBlockV[i-headPadSize];
    }
    
    // wrap it in an IValue
    std::vector<torch::jit::IValue> inBufferI = {inBufferT};
    // or maybe this which avoids the copy constructor which will be a good idea in an
    // realtime audio loop!
    // std::vector<torch::jit::IValue> inBufferI = {torch::jit::IValue::make_capsule(inBufferT)};
    // and ... inference
    // std::cout << "inf start" << std::endl;
    
    // copy 0-blockSize to the out buffer
    torch::Tensor outT = module.forward(inBufferI, conditioningParams).toTensor() * 0.25; // 0.25 to top clipping
 
    // module.forward(inBufferI, conditioningParams);
    
    // now sort out the copying of the tail
    // blockSize -> tailSize is the reverb tail
    // std::cout << "inf done" << std::endl;
    // std::cout << "post proc" << std::endl;
    // copy the first block of output to output block, which is gonna get played
    std::memcpy(outBlockV.data(), outT.data_ptr(), sizeof(float)*blockSize);
    // copy the whole output into the temp block to speed up the processing below
    std::memcpy(outputTempV.data(), outT.data_ptr(), sizeof(float)*outputTempV.size());
      //sum the previous tail to out buff (tail from previous block process)
    for (auto i=0;i<blockSize;++i){
        outBlockV[i] += tailBlockV[i];
    }
    // sum new tail[0->(end-blocksize)]  to old tail [blockSize->end] into tailBlockV
    auto oldTailL = tailPadSize-blockSize;
    for (auto i=0; i<oldTailL; ++i){
        tailBlockV[i] = tailBlockV[i+blockSize];// shunt old tail along
      // straight from the tensor is slowwww
        //tailBlockV[i] += outT[0][0][i+blockSize].item<float>();// sum with new tail
        tailBlockV[i] += outputTempV[i+blockSize];
    }
    // add the final bit of new tail to the end
    for (auto i=tailPadSize-blockSize; i<tailPadSize; ++i){
      // straight from the tensor is slowwww
        // tailBlockV[i] += outT[0][0][i+blockSize].item<float>();// sum with new tail
        tailBlockV[i] += outputTempV[i+blockSize];
    }
    // std::cout << "post proc done" << std::endl;

    // append final bit of new tail (end-blocksize -> end) to tailBlockV


}

void testMemcpy(){
    // memcpy from tensor to vector
    torch::Tensor t = torch::ones({100}, torch::kFloat32);
    std::vector<float> v (100, 0);
    assert(v[0] == 0);
    std::memcpy(v.data(), t.data_ptr(), sizeof(float)*100);
    assert(v[0] == 1);
    // memcpy from vector to tensor
    torch::Tensor t2 = torch::ones({100}, torch::kFloat32);
    assert(t2[50].item<float>() == 1.0);

    std::vector<float> v2 (100, 0);
    v2[50] = 0.5;
    std::memcpy(t2.data_ptr(), v2.data(), sizeof(float)*100);
    assert(t2[50].item<float>() == 0.5);
}

int main(int argc, const char* argv[]) {

  if (argc != 4) {
    std::cerr << "usage: verb-app <path-to-exported-script-module> input wav, output wav\n";
    return -1;
  }
 
    // load the audio 
    std::vector<float> audio = tinywav_loadWav(argv[2]);
    

    // create the various blocks and such
    torch::jit::script::Module module = torch::jit::load(argv[1]);;
    long blockSize = 1024 * 8;
    long headPadSize = 88889-1;
    long tailPadSize = 88889-1;
    torch::Tensor inBufferT = torch::zeros ({1, 1, headPadSize+blockSize+tailPadSize}, torch::kFloat32);
    //torch::Tensor outBufferT = torch::zeros ({1, 1, blockSize+tailPadSize}, torch::kFloat32);
    std::vector<float> outputTempV(blockSize + tailPadSize, 0);
    std::vector<float> outBlockV (blockSize, 0);
    std::vector<float> tailBlockV (tailPadSize, 0);
    torch::jit::Kwargs c = getConditioning(0.5, -0.5);// 0.5,-0.5 is a nice obvious verb
    
    
    std::vector<float> audio_out (audio.size() + tailPadSize, 0);
    torch::NoGradGuard no_grad;
    torch::jit::GraphOptimizerEnabledGuard
    // pass a block through processblock
    for (auto start=0;start < audio.size() - blockSize; start += blockSize){
      auto startTime = std::chrono::high_resolution_clock::now();
      double minTime = (double) blockSize / 44100.0; 
      std::vector<float> inBlockV(audio.begin()+start, audio.begin() + start + blockSize);
      processBlock(module,
                    blockSize, // signal length (len inBlock)
                    headPadSize, // zero padding to prefix inBlock 
                    tailPadSize, // zero padding to append inBlock
                    inBufferT, // reusable input tensor should be size 1,1,(blockSize+headPad+tailPad)
                    c, // conditioning
                    //outBufferT, // copy tensor output to here maybe
                    inBlockV, 
                    outBlockV, // copy signal.length of result here 
                    tailBlockV,  // copy bit after signal.length of result here after pullingtailPadSize off the start and adding to outBlock
                    outputTempV
                  );
      
      auto endTime = std::chrono::high_resolution_clock::now();

      // Calculate the duration in microseconds
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

      // Print the duration
      std::cout << "Time taken by the function: " << (duration/1000000.0f) << " seconds" << " min time "<< minTime << std::endl;
      // copy outblock to the audio out vector
      std::copy_n(outBlockV.begin(), blockSize, audio_out.begin() + start);
      std::cout << "done up to " << (start + blockSize) << " of " << audio_out.size() << std::endl;
    }
    // now add the last tail 
    // std::cout << "Adding tail " << 
    std::copy_n(tailBlockV.begin(), tailPadSize, audio_out.begin() + (audio_out.size() - tailPadSize - blockSize));

    tinywav_myk_tiny::saveWav(audio_out, 1, 44100, argv[3]);
}