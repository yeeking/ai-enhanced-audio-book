import myk_data
import soundfile
import torch 

def run_file_through_model(model, infile, outfile, samplerate=44100, device='cpu'):
    """
    read the sent file from disk, pass it through the model
    and back out to the other file 
    """
    indata = myk_data.load_wav_file(infile, want_samplerate=samplerate)
    # model expects (which seq, which sample, which channel) so reshape
    indata = indata.reshape((1, indata.shape[0], 1))
    outputs = model.forward(torch.tensor(indata).to(device))
    # reshape back to [[], [], []]
    outputs = outputs.reshape((outputs.shape[1], 1))
    outputs = outputs.cpu().detach().numpy()
    soundfile.write(outfile, outputs, samplerate)
    
