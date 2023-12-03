import torch

# set random seed so it generates the 
# same network parameters 
# torch.manual_seed(10)
# make a simple LSTM network layer
my_lstm = torch.nn.LSTM(1, 1, 1)
in_t = torch.zeros((100,1))
in_t[0][0] = 1.0
                   
# pass it through the LSTM layer
out_t,hx = my_lstm.forward(in_t)
# print the results
print(out_t)