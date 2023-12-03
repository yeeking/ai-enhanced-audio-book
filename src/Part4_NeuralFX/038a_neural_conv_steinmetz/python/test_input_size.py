import torch
from models import TCN, TCNBlock, FiLM
import numpy as np
import torchaudio
import time 

process_file = "../../audio/drums_16bit.wav"
#process_file = "qq.wav"
#!wget https://csteinmetz1.github.io/steerable-nafx/models/reverb_full.pt > /dev/null
model_verb = torch.load("../models/reverb_full.pt", map_location="cpu").eval()

rf_size = model_verb.compute_receptive_field()
#c = torch.rand(1, 2).view(1, 1, -1)
c = torch.tensor([0.5, -0.5]).view(1, 1, -1)
block_size = 4096
x = torch.rand(block_size).view(1,1,-1)

x_pad = torch.nn.functional.pad(x, (rf_size, rf_size))
# x_pad = torch.nn.functional.pad(x, (rf_size, 0))

with torch.no_grad():
    for i in range(50):
        start_time = time.time()

        out = model_verb(x_pad, c)

        end_time = time.time()

        execution_time = end_time - start_time
        print(f"Inference time:{block_size:.6f} {execution_time:.6f} seconds")


print("in shape", x_pad.shape, "out shape", out.shape)
torchaudio.save("output,wav", out.view(1,-1), 44100, compression=320.0)
#torchaudio.save("output.mp3", x_pad.view(1,-1), 44100, compression=320.0)



