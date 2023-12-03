
import torch
import numpy as np
from scipy.io.wavfile import write

torch.manual_seed(10)

# synthesize a 400Hz sine at 44,100Hz sample rate
freq = 400
clean_sine = np.sin(
      np.arange( 
         0, np.pi * freq*2, 
         (np.pi * freq * 2) /44100), 
         dtype=np.float32)

# reshape it so each sample is in its own 
# box [[], [] ... []]
clean_sine = np.reshape(
                clean_sine, 
                (len(clean_sine), 1))

my_lstm = torch.nn.LSTM(1, 1, 1)
in_t = torch.tensor(clean_sine)

# pass it through the LSTM layer
out_t,hx = my_lstm.forward(in_t)
# print the results
print(out_t)

write('sine_400.wav', 44100, clean_sine * 0.5)
write('sine_400-lstm.wav', 44100, out_t.detach().numpy())