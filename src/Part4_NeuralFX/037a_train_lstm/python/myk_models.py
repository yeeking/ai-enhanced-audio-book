## A. Wright, E.-P. Damskägg, and V. Välimäki, ‘Real-time black-box modelling with recurrent neural networks’, in 22nd international conference on digital audio effects (DAFx-19), 2019, pp. 1–8.

import torch 
import json
from json import JSONEncoder

class SimpleLSTM(torch.nn.Module):
    """
    LSTM Model after
    A. Wright, E.-P. Damskägg, and V. Välimäki, ‘Real-time black-box modelling with recurrent neural networks’, in 22nd international conference on digital audio effects (DAFx-19), 2019, pp. 1–8.
    uses 32 hidden by default.
    Wright et al. showed decent performance for 32, but 
    even better going up to 96
    """
    def __init__(self, hidden_size=32):
        super().__init__()
        # Batch first means input data is [batch,sequence,feature]
        self.lstm = torch.nn.LSTM(input_size=1, hidden_size=hidden_size, num_layers=1, batch_first=True)
        self.dense = torch.nn.Linear(hidden_size, 1)# from 8 hidden back to 1 output
        self.drop_hidden = True
        self.hidden = torch.zeros((1))
        self.cell = torch.zeros((1))


    def zero_on_next_forward(self):
        """
        next time forward is called, the network will
        run it with zeroed hidden+cell values. Cannot zero them here as only forward knows
        the shape of the input and therefore the hidden and cell states. 
        """
        self.drop_hidden = True 
    
    def forward(self, torch_in):
        """
        compute the output. Hidden and cell states are retained unless you called 
        zero_on_next_forward before calling forward.
        """
        correct_state_shape = (1, torch_in.shape[0], self.lstm.hidden_size)
        # if input size has changed or they called zero_on_next_forward
        # then zero out the hidden and cell values 
        if (self.drop_hidden) or (self.hidden.shape != correct_state_shape): # reset hidden states
            self.hidden = torch.zeros(correct_state_shape).to(torch_in.device)
            self.cell = torch.zeros(correct_state_shape).to(torch_in.device)
        if self.drop_hidden:
            self.drop_hidden = False

        # forward, storing new state to h and c
        x, (h, c) = self.lstm(torch_in, (self.hidden, self.cell)) 
        # store the state to the object's self. fields
        # clone and detach are needed to remove any gradients that have been 'backwarded' by a loss func
        self.hidden = h.clone().detach() 
        self.cell = c.clone().detach()
        return self.dense(x)
    
    def save_for_rtneural(self, outfile):
        """
        saves the network weights to a JSON file suitable for import to rtneural
        """ 
        # class to use to convert data from tensor format to simple Python lists 
        class EncodeTensor(JSONEncoder):
            def default(self, obj):
                if isinstance(obj, torch.Tensor):
                    return obj.cpu().detach().numpy().tolist()
                return super(json.NpEncoder, self).default(obj)
            
        with open(outfile, 'w') as json_file:
            json.dump(self.state_dict(), json_file,cls=EncodeTensor)

    