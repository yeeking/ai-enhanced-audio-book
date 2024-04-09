#include "../../tinywav/myk_tiny.h"
#include <string> 
#include <vector>
#include <iostream> 


int main(){

        float a = -0.9;
    float b = 0.5; // feedback
    float x[] = {0.1, -0.1, 0.2, 0.5, 0.25, 0}; // signal
    float y[100]; // output - we'll run it for 100 samples
    y[0] = 0;
    for (int n=1;n<100;++n){
        float xn = 0; // zero pad 
        if (n < 6) xn = x[n]; // unless there x has a value for n 
        y[n] = a*y[n-1] + b*xn;
        printf("x[%i]=%f y[%i]=%f\n", n, xn, n, y[n]);
    }
}

