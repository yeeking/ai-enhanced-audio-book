## Export several models for performance testing 

import sys
import torch 

sys.path.append('../../037a_train_lstm/python')

import myk_models

for i in [1, 2, 4, 8, 16, 32, 64, 128, 256, 512]:
    json_file = "../models/test_" + str(i) + ".json"
    tscript_file = "../models/test_" + str(i) + ".ts"
    
    model = myk_models.SimpleLSTM(i)
    model.save_for_rtneural(json_file)
    print("Saving torchscript pt fo ", tscript_file)
    model.eval()
    scripted_model = torch.jit.script(model)
    torch.jit.save(scripted_model, tscript_file)


