import torch

torch.manual_seed(21)

my_lstm = torch.nn.LSTM(1, 1, 1)
input = torch.rand(1, 1)
h0 = torch.rand(1, 1)
c0 = torch.rand(1, 1)

traced_lstm = torch.jit.trace(my_lstm, (input, (h0, c0)))
traced_lstm.save('my_lstm.pt')

output, state = my_lstm.forward(input)

# output, state = traced_lstm.forward(input, (h0,c0))
output, state = traced_lstm.forward(input, state)

print(output)
