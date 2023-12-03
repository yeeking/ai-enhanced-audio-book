import torch


input_size = 1 # mono signal
hidden_size = 1
num_layers = 1
seq_length = 5
# # torch.nn.LSTM(input_size, hidden_size, num_layers=1, 
# model = torch.nn.LSTM(input_size, hidden_size, num_layers)
# inp = torch.rand((1, input_size))
# # for reference, here is how the hidden sizes are set: 
# h0 = torch.rand(num_layers, hidden_size)
# c0 = torch.rand(num_layers, hidden_size)
# out,state = model.forward(inp, (h0,c0))


torch.manual_seed(21)
orig_lstm = torch.nn.LSTM(input_size, hidden_size, num_layers)
input = torch.rand(seq_length, input_size) 
h0 = torch.rand(num_layers, hidden_size)
c0 = torch.rand(num_layers, hidden_size)

traced_lstm = torch.jit.trace(orig_lstm, (input))
traced_lstm.save('my_lstm_no_state.pt')

traced_lstm = torch.jit.trace(orig_lstm, (input, (h0, c0)))
traced_lstm.save('my_lstm_with_state.pt')
