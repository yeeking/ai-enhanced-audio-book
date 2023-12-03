import torch
from torch.utils.data import DataLoader
import numpy as np
import json
from json import JSONEncoder
import myk_models

def get_device():
    if not torch.cuda.is_available():
        print('cuda device not available/not selected')
        device = 'cpu'
    else:
        #torch.set_default_tensor_type('torch.cuda.FloatTensor')
        torch.cuda.set_device(0)
        print('cuda device available')
        device = 'cuda'
    return device     

def train_epoch(model : myk_models.SimpleLSTM,#torch.nn.Module, 
                dataloader : DataLoader,  
                loss_functions, 
                optimiser, 
                device='cpu',
                warm_up_len=1000):
    """
    warm_up_len=how many samples to feed in before calculating loss so the delay line can warm up(!)
    """
    batch_losses = []
    # note that if the dataloader has shuffle set to true
    # it will shuffle the batches every time you create 
    # an iterator 
    for batch_idx, (inputs, targets) in enumerate(dataloader):
        inputs, targets = inputs.to(device), targets.to(device)
    
        # warm up. shape: [batch,sequence,feature]   
        model.zero_on_next_forward()  # tell it to zero its hiddens
        model.forward(inputs[:, 0:warm_up_len, :]) # feed it
        model.zero_grad() # zero out the gradients
        
        # send in the inputs, skipping the warm up sequence
        outputs = model.forward(inputs[:,warm_up_len:, :])
        
        loss = loss_functions(outputs, targets[:,warm_up_len:, :])
        loss.backward()
        optimiser.step()
        model.zero_grad()


        batch_losses.append(loss.cpu().detach().numpy())

        ## should probably reset the hidden layers on the LSTM here
        # model.somehow_reset_hidden_layers??
    ep_loss = np.mean(batch_losses)
    return ep_loss


def train_epoch_interval(model : myk_models.SimpleLSTM,#torch.nn.Module, 
                dataloader : DataLoader,  
                loss_functions, 
                optimiser, 
                device='cpu',
                warm_up_len=1000,
                sub_batch_seq_len=2048):
    """
    warm_up_len=how many samples to feed in before calculating loss so the delay line can warm up(!)
    update_interval=how many samples to process before updating weights within a batch I think relates to Truncated BPTT
    """
    batch_losses = []
    # note that if the dataloader has shuffle set to true
    # it will shuffle the batches every time you create 
    # an iterator 
    for batch_idx, (inputs, targets) in enumerate(dataloader):
        inputs, targets = inputs.to(device), targets.to(device)
    
        # warm up. shape: [batch,sequence,feature]
        # print("Warm up")
        model.zero_on_next_forward()  
        model.forward(inputs[:, 0:warm_up_len, :])
        model.zero_grad()
        # now we iterate over in chunks of 2048
        # training at each step
        available_sub_batches = int(np.floor(inputs.shape[1] / sub_batch_seq_len))
        start_sample = warm_up_len
        end_sample = warm_up_len + sub_batch_seq_len
        # send in the intervallic sub batches / sub sequences really 
        for sub_batch_ind in range(available_sub_batches):
            # print("B", batch_idx, "sb", sub_batch_ind, "s", start_sample, "e", end_sample)
            outputs = model.forward(inputs[:,start_sample:end_sample, :])
            # send in the inputs, skipping the warm up sequence
            loss = loss_functions(outputs, targets[:,start_sample:end_sample, :])
            loss.backward()
            # schedule version 
            # optimiser.step(loss)
            optimiser.step()
            model.zero_grad()

            start_sample += sub_batch_seq_len
            end_sample = start_sample + sub_batch_seq_len

            batch_losses.append(loss.cpu().detach().numpy())

    ep_loss = np.mean(batch_losses)
    return ep_loss



def compute_batch_loss(model : torch.nn.Module, 
                dataloader : DataLoader,  
                loss_functions, 
                device='cpu',
                warm_up_len=200):
    
    batch_losses = []

    for batch_idx, (inputs, targets) in enumerate(dataloader):

        inputs, targets = inputs.to(device), targets.to(device)
        model.zero_on_next_forward()  
        model.forward(inputs[:, 0:warm_up_len, :])
        # clear the gradiants from the warm up
        model.zero_grad()
        
        # send in the inputs
        outputs = model.forward(inputs[:, warm_up_len:, :])
        loss = loss_functions(outputs, targets[:, warm_up_len:, :])
        batch_losses.append(loss.cpu().detach().numpy())

    ep_loss = np.mean(batch_losses)
    return ep_loss

