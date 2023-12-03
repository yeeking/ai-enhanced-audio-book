import torch

torch.manual_seed(21)
my_lstm = torch.nn.LSTM(1, 1, 1)
traced_lstm = torch.jit.trace(my_lstm, (torch.rand(1, 1),))
traced_lstm.save('my_lstm.pt')
