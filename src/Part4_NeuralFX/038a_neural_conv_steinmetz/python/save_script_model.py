import torch
from models import TCN, TCNBlock, FiLM
process_file = "../../audio/drums_16bit.wav"
#process_file = "qq.wav"
#!wget https://csteinmetz1.github.io/steerable-nafx/models/reverb_full.pt > /dev/null
model_verb = torch.load("../models/reverb_full.pt", map_location="cpu").eval()


## try and trace it then save out
# this dictates the input size
rf_size = model_verb.compute_receptive_field()
## double the receptive field size. not sure why?? 
# trace_input = torch.rand(rf_size*2).view(1,1,-1)
trace_input = torch.rand(rf_size*2).view(1,1,-1)

## the conditioning params
trace_input_c = torch.rand(1, 2).view(1, 1, -1)
## trace it - note that steinmetz' model passes the conditioning
## in as a second parameter instead of as another value in parallel with the input 
## signal meaning we need to use the example_kwarg_inputs param
## to pass it the main input x and conditioning i.e params 
traced_script_module = torch.jit.trace(model_verb, example_kwarg_inputs={"x":trace_input, "c":trace_input_c})
traced_script_module.save("../models/traced_reverb_"+str(rf_size)+".pt")

