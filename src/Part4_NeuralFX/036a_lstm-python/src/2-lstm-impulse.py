import torch

# set random seed so it generates the 
# same network parameters 
torch.manual_seed(21)
# make a simple LSTM network layer
my_lstm = torch.nn.LSTM(1, 1, 1)
# make an input
in_t = torch.tensor([[1.0], [0], [0], [0], [0], [0]])
# pass it through the LSTM layer
out_t,hx = my_lstm.forward(in_t)
# print the results
print(out_t)