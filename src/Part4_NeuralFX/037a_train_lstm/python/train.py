## This script trains an LSTM according
## to the method described in 
## A. Wright, E.-P. Damskägg, and V. Välimäki, ‘Real-time black-box modelling with recurrent neural networks’, in 22nd international conference on digital audio effects (DAFx-19), 2019, pp. 1–8.
import myk_data
import myk_models
import myk_loss 
import myk_train
import torch 
from torch.utils.data import DataLoader
import myk_evaluate
import os 
import sys 

from torch.utils.tensorboard import SummaryWriter

# used for the writing of example outputs
run_name="ssl-32-unit"
# dataset : need an input and output folder in this folder
#audio_folder = "../../data/audio_ssl/"
# this is a  dataset made with a simple audacity distortion effect
audio_folder = "../../data/audio_audacity_dist"
assert os.path.exists(audio_folder), "Audio folder  not found. Looked for " + audio_folder
# used to render example output during training
test_file = "../../data/audio_ht1/guitar.wav"
assert os.path.exists(test_file), "Test file not found. Looked for " + test_file

# try to read neural network size from first command-line input
if len(sys.argv) == 1:
    lstm_hidden_size = 32
else:
    lstm_hidden_size = int(sys.argv[1])

# Training configuration 
learning_rate = 5e-3 # how much to adjust network by each time 
seq_length_secs = 0.5 # length of audio in a sequence 
batch_size = 50 # number of sequences in a batch (how parallel is it?)
max_epochs = 10000
give_up_after = 1000 # stop training if no improvement for this many epochs

# create the logger for tensorboard
expt_desc = " " + run_name + " LSTM model with " + str(lstm_hidden_size) + " hidden units"
writer = SummaryWriter(comment=expt_desc)

print("Loading dataset from folder ", audio_folder)

dataset = myk_data.generate_dataset(audio_folder + "/input/", audio_folder + "/output/", frag_len_seconds=0.5)

print("Splitting dataset")
train_ds, val_ds, test_ds = myk_data.get_train_valid_test_datasets(dataset, splits=[0.8, 0.1, 0.1])

print("Looking for GPU power")
device = myk_train.get_device()
# device = 'cpu'

print("Creating data loaders")

train_dl = DataLoader(train_ds, batch_size=batch_size, shuffle=True)
val_dl = DataLoader(val_ds, batch_size=batch_size, shuffle=True)
test_dl = DataLoader(test_ds, batch_size=batch_size, shuffle=True)

print("Creating model")
model = myk_models.SimpleLSTM(hidden_size=lstm_hidden_size).to(device)

print("Creating optimiser")
# https://github.com/Alec-Wright/Automated-GuitarAmpModelling/blob/main/dist_model_recnet.py
optimiser = torch.optim.Adam(model.parameters(), lr=learning_rate, weight_decay=1e-4)
# scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimiser, 'min', factor=0.5, patience=5, verbose=True)
scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimiser, 'min', factor=0.5, patience=5, verbose=False)

print("Creating loss functions")
# https://github.com/Alec-Wright/CoreAudioML/blob/bad9469f94a2fa63a50d70ff75f5eff2208ba03f/training.py
loss_functions = myk_loss.LossWrapper()
# now the training loop

print("About to train")
lowest_val_loss = 0
best_loss = False
epochs_no_improvement = 0
model_save_dir = writer.get_logdir() + "/saved_models/"
os.makedirs(os.path.dirname(model_save_dir), exist_ok=True)
    
for epoch in range(max_epochs):
    # ep_loss = myk_train.train_epoch_interval(model, train_dl, loss_functions, scheduler, device=device)
    ep_loss = myk_train.train_epoch_interval(model, train_dl, loss_functions, optimiser, device=device)
    
    #ep_loss = myk_train.train_epoch(model, train_dl, loss_functions, optimiser, device=device)
    val_loss = myk_train.compute_batch_loss(model, val_dl, loss_functions, device=device)
    writer.add_scalar("Loss/val", val_loss, epoch)
    writer.add_scalar("Loss/train", ep_loss, epoch)
    
    # check if we have beaten our best loss to date
    if lowest_val_loss == 0:# first run
        lowest_val_loss = val_loss
    elif val_loss < lowest_val_loss:# new record
        lowest_val_loss = val_loss
        best_loss = True
    else:# no improvement
        best_loss = False
    if best_loss == False:
        epochs_no_improvement = epochs_no_improvement + 1
        if epochs_no_improvement > give_up_after:
            print(str(give_up_after) + " epochs with no improvement. Giving up")
            break
    else:
        epochs_no_improvement = 0

    if (best_loss) or (epoch % 250 == 0):# save best model 
        print("Record loss - saving at ", epoch)
        # pytorch save
        pth_file = model_save_dir + 'lstm_size_' + str(lstm_hidden_size) + '_epoch_' + str(epoch) + "_loss_" + str(round(val_loss, 4)) + ".pth"
        torch.save(model, pth_file)
        # run some audio through the model and save as wav
        myk_evaluate.run_file_through_model(model, test_file, model_save_dir + "/" + str(epoch)+".wav", device=device)
    if best_loss: # rtneural json save is always the best yet 
        model.save_for_rtneural(model_save_dir+"rtneural_model_lstm_"+str(lstm_hidden_size)+".json")
 
    print("epoch, train, val ", epoch, ep_loss, val_loss)
    

