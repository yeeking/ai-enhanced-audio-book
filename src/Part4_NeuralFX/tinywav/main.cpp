#include <vector>
#include <string>
#include <iostream>
#include "myk_tiny.h"

int main() {
    std::string inFilePath{"../../audio/toby.wav"};
    std::string outFilePath{"../../audio/test.wav"};

    std::cout << "Reading file " << inFilePath << std::endl;
    std::vector<float> data = myk_tiny::loadWav(inFilePath);

    // check for all zeros
    bool all_zero = true;
    for (float& f  : data){
        if (f != 0) {
            all_zero = false;
            break;
        }
    }
    if (all_zero) {
        std::cout << "Test failed all data is zero " << std::endl;
        return 1;
    }

    std::cout << "Writing file " << outFilePath << std::endl;
    myk_tiny::saveWav(data, 1, 44100, outFilePath);
    std::cout << "Reading new file back in to compare to original " << outFilePath << std::endl;
    std::vector<float> datav2 = myk_tiny::loadWav(outFilePath);
    if (datav2.size() != data.size()){
        std::cout << "Test failed data length different " << std::endl;
        return 1; 
    }
    for (auto i=0;i<data.size();++i){
        if (datav2[i] != data[i]){
            std::cout << "Test failed at index " << i << " want " << data[i] << " got " << datav2[i] << std::endl;
            return 1;
        }
    }
    std::cout << "All tests passed somehow" << std::endl;
}
