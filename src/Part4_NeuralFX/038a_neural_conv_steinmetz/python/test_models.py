import torch
from models import TCN, TCNBlock, FiLM
import torchaudio
import scipy
#########################
#### runnnn it 
#########################

process_file = "../../audio/drums_16bit.wav"
#process_file = "qq.wav"
#!wget https://csteinmetz1.github.io/steerable-nafx/models/reverb_full.pt > /dev/null
model_verb = torch.load("../models/reverb_full.pt", map_location="cpu").eval()
x_p, sample_rate = torchaudio.load(process_file)
# x_p = torch.rand(1,  1)
print("Loaded file. shape", x_p.shape)

# effect_type - Choose from one of the pre-trained models.
# gain_dB - Adjust the input gain. This can have a big effect since the effects are very nonlinear.
# c0 and c1 - These are the effect controls which will adjust perceptual aspects of the effect, depending on the effect type. Very large values will often result in more extreme effects.
# mix - Control the wet/dry mix of the effect.
# width - Increase stereo width of the effect.
# max_length - If you uploaded a very long file this will truncate it.
# stereo - Convert mono input to stereo output.
# tail - If checked, we will also compute the effect tail (nice for reverbs).

effect_type = "Reverb" #@param ["Compressor", "Reverb", "Amp", "Analog Delay", "Synth2Synth"]
gain_dB = -24 #@param {type:"slider", min:-24, max:24, step:0.1}
c0 = -1.4 #@param {type:"slider", min:-10, max:10, step:0.1}
c1 = 3 #@param {type:"slider", min:-10, max:10, step:0.1}
mix = 70 #@param {type:"slider", min:0, max:100, step:1}
width = 50 #@param {type:"slider", min:0, max:100, step:1}
max_length = 60 #@param {type:"slider", min:5, max:120, step:1}
stereo = True #@param {type:"boolean"}
tail = True #@param {type:"boolean"}

# select model type
if effect_type == "Reverb":
  pt_model = model_verb
# elif effect_type == "Compressor":
#   pt_model = model_comp
# elif effect_type == "Amp":
#   pt_model = model_amp
# elif effect_type == "Analog Delay":
#   pt_model = model_delay
# elif effect_type == "Synth2Synth":
#   pt_model = model_synth

# measure the receptive field
pt_model_rf = pt_model.compute_receptive_field()
print("receptive field", pt_model_rf)
# crop input signal if needed
max_samples = int(sample_rate * max_length)
x_p_crop = x_p[:,:max_samples]
print("cropped the input to ", x_p_crop.shape)
chs = x_p_crop.shape[0]

# if mono and stereo requested
if chs == 1 and stereo:
  x_p_crop = x_p_crop.repeat(2,1)
  chs = 2

# pad the input signal
front_pad = pt_model_rf-1
back_pad = 0 if not tail else front_pad
x_p_pad = torch.nn.functional.pad(x_p_crop, (front_pad, back_pad))
print("front pad", front_pad, "back pad", back_pad, "input shape ", x_p_pad.shape)
# design highpass filter
sos = scipy.signal.butter(
    8, 
    20.0, 
    fs=sample_rate, 
    output="sos", 
    btype="highpass"
)

# compute linear gain 
gain_ln = 10 ** (gain_dB / 20.0)

# process audio with pre-trained model
with torch.no_grad():
  y_hat = torch.zeros(x_p_crop.shape[0], x_p_crop.shape[1] + back_pad)
  print("y hat is", y_hat.shape)
  for n in range(chs):
    if n == 0:
      factor = (width*5e-3)
    elif n == 1:
      factor = -(width*5e-3)
    c = torch.tensor([float(c0+factor), float(c1+factor)]).view(1,1,-1)
    print("doing inference with some conditioning", c, "reshaped inputfrom", x_p_pad.shape, "to", x_p_pad[n,:].view(1,1,-1).shape, "offset is ", n)
    #print("example input is ", x_p_pad[n,:].view(1,1,-1)[0])
    y_hat_ch = pt_model(gain_ln * x_p_pad[n,:].view(1,1,-1), c)
    print("Received output of shape ", y_hat_ch.shape)
    y_hat_ch = scipy.signal.sosfilt(sos, y_hat_ch.view(-1).numpy())
    y_hat_ch = torch.tensor(y_hat_ch)
    y_hat[n,:] = y_hat_ch

# pad the dry signal 
x_dry = torch.nn.functional.pad(x_p_crop, (0,back_pad))

# normalize each first
y_hat /= y_hat.abs().max()
x_dry /= x_dry.abs().max()

# mix
mix = mix/100.0
y_hat = (mix * y_hat) + ((1-mix) * x_dry)

# remove transient
y_hat = y_hat[...,8192:]
y_hat /= y_hat.abs().max()
print("Saving to output.mp3")
torchaudio.save("output.mp3", y_hat.view(chs,-1), sample_rate, compression=320.0)
print("Done.")

