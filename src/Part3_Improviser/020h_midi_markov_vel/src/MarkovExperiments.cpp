#include "../../MarkovModelCPP/src/MarkovManager.h"
#include <iostream>


int main(){
    MarkovManager mm{};
    mm.putEvent("C");
    mm.putEvent("D");
    mm.putEvent("E");
    mm.putEvent("F");
    mm.putEvent("C");
    mm.putEvent("F");
    
    
    
    std::cout << mm.getModelAsString() << std::endl;
    return 0;
}
