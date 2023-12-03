int main(){
    // number of coefficients
    int N = 3;
    // length of signals
    int sig_len = 6;
    // set of coeffients of length N
    float b[] = {0.25, 0.5, 0.25};
    // input signal of length sig_len
    float x[] = {0.1,-0.1,0.2,0.5,0.25,0.1};
    // zeroed out signal of length same as signal
    float y[] = {0,0,0,0,0,0}; // output
    // iterate over the signal, start N steps in
    for (int n=N;n<sig_len;++n){
        // iterate over the coefficients
        // this is what the expression above describes
        for (int i=0;i<N;++i){
            y[n] += b[i]*x[n-i];
        }
    }
}
