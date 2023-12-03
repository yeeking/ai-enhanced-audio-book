#include "MarkovChain.h"
#include "MarkovManager.h"
//#include "dinvernoSystem.h"
//#include "../JuceLibraryCode/JuceHeader.h"

#include <iostream>
#include <string>
#include <random>

int main(){
     MarkovManager man{};
    for (auto i=0; i<10000; ++i){
        man.putEvent("s_"+std::to_string(i));
    }
}
