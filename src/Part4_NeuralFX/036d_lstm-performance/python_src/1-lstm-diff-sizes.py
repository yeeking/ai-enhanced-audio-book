import torch
torch.manual_seed(21)
for i in range(10):
    # increase network hidden size exponentially
    # so 1,2,4...128
    my_lstm = torch.nn.LSTM(1, pow(2, i), 1)
    traced_lstm = torch.jit.trace(
                    my_lstm, 
                    torch.rand(1, 1)
                )
    fname = 'my_lstm_' + str(pow(2, i)) + '.pt'
    print("saving ", fname)
    traced_lstm.save(fname)
