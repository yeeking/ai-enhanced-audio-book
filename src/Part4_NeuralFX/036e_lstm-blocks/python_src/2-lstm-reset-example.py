import torch

torch.manual_seed(21)
my_lstm = torch.nn.LSTM(1, 1, 1)

input = torch.tensor([[0.5]])
output, _ = my_lstm.forward(input)
print(output)

input = torch.tensor([[0.1]])
output, _ = my_lstm.forward(input)
print(output)

input = torch.tensor([[0.5], [0.1]])
output, _ = my_lstm.forward(input)
print(output)

## now remember state

input = torch.tensor([[0.5]])
output, state = my_lstm.forward(input)
print(output)

input = torch.tensor([[0.1]])
output, state = my_lstm.forward(input, state)
print(output)

input = torch.tensor([[0.5], [0.1]])
output, _ = my_lstm.forward(input)
print(output)




