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
    indata_tensor = torch.tensor(indata, dtype=torch.float32).to(device)
    model.zero_on_next_forward()

    # Process in chunks
    outputs = []
    start = 0
    chunk_size = 2048 * 16
    while start < indata_tensor.shape[1]:
        end = min(start + chunk_size, indata_tensor.shape[1])
        chunk = indata_tensor[:, start:end, :]

        # Forward pass
        out_chunk = model.forward(chunk)
        outputs.append(out_chunk)

        start = end

    outputs = torch.cat(outputs, dim=1)
    # # outputs = model.forward(indata_tensor)
    # # reshape back to [[], [], []]
    outputs = outputs.reshape((outputs.shape[1], 1))
    outputs = outputs.cpu().detach().numpy()
    soundfile.write(outfile, outputs, samplerate)
    
