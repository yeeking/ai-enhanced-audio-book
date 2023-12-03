## this script loads in a trained lstm model
## based on the myk_models.SimpleLSTM class
## and exports it to JSON for RTNeural 
## AND torchscript format 

import sys
import os 
import torch 

myk_classes = "../037a_train_lstm/python/"
pth_file = "./models/dist_32.pth"
json_file = "./models/dist_32.json"
tscript_file = "./models/dist_32.ts"

assert os.path.exists(myk_classes), 'Cannot find myk_models folder ' + myk_classes
sys.path.append(myk_classes)
import myk_models

assert os.path.exists(pth_file), "Cannot find requested pth file: " + pth_file

print("loading model spec from ", pth_file)
model = torch.load(pth_file)

print("Saving rtneural json to ", json_file)
model.save_for_rtneural(json_file)

print("Saving torchscript pt fo ", tscript_file)
model.eval()
scripted_model = torch.jit.script(model)
torch.jit.save(scripted_model, tscript_file)

