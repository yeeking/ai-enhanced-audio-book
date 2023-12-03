import torch 
import torchaudio 
import scipy 

### All code in this file pretty much verbatim from: 
### https://github.com/csteinmetz1/steerable-nafx
### Christian Steinmetz
# @inproceedings{steinmetz2021steerable,
# title={Steerable discovery of neural audio effects},
# author={Steinmetz, Christian J. and Reiss, Joshua D.},
# booktitle={5th Workshop on Creativity and Design at NeurIPS},
# year={2021}}


def causal_crop(x, length: int):
    if x.shape[-1] != length:
        stop = x.shape[-1] - 1
        start = stop - length
        x = x[..., start:stop]
    return x

class FiLM(torch.nn.Module):
    def __init__(
        self,
        cond_dim,  # dim of conditioning input
        num_features,  # dim of the conv channel
        batch_norm=True,
    ):
        super().__init__()
        self.num_features = num_features
        self.batch_norm = batch_norm
        if batch_norm:
            self.bn = torch.nn.BatchNorm1d(num_features, affine=False)
        self.adaptor = torch.nn.Linear(cond_dim, num_features * 2)

    def forward(self, x, cond):

        cond = self.adaptor(cond)
        g, b = torch.chunk(cond, 2, dim=-1)
        g = g.permute(0, 2, 1)
        b = b.permute(0, 2, 1)

        if self.batch_norm:
            x = self.bn(x)  # apply BatchNorm without affine
        x = (x * g) + b  # then apply conditional affine

        return x

class TCNBlock(torch.nn.Module):
  def __init__(self, in_channels, out_channels, kernel_size, dilation, cond_dim=0, activation=True):
    super().__init__()
    self.conv = torch.nn.Conv1d(
        in_channels, 
        out_channels, 
        kernel_size, 
        dilation=dilation, 
        padding=0, #((kernel_size-1)//2)*dilation,
        bias=True)
    if cond_dim > 0:
      self.film = FiLM(cond_dim, out_channels, batch_norm=False)
    if activation:
      #self.act = torch.nn.Tanh()
      self.act = torch.nn.PReLU()
    self.res = torch.nn.Conv1d(in_channels, out_channels, 1, bias=False)

  def forward(self, x, c=None):
    x_in = x
    x = self.conv(x)
    if hasattr(self, "film"):
      x = self.film(x, c)
    if hasattr(self, "act"):
      x = self.act(x)
    x_res = causal_crop(self.res(x_in), x.shape[-1])
    x = x + x_res

    return x

class TCN(torch.nn.Module):
  def __init__(self, n_inputs=1, n_outputs=1, n_blocks=10, kernel_size=13, n_channels=64, dilation_growth=4, cond_dim=0):
    super().__init__()
    self.kernel_size = kernel_size
    self.n_channels = n_channels
    self.dilation_growth = dilation_growth
    self.n_blocks = n_blocks
    self.stack_size = n_blocks

    self.blocks = torch.nn.ModuleList()
    for n in range(n_blocks):
      if n == 0:
        in_ch = n_inputs
        out_ch = n_channels
        act = True
      elif (n+1) == n_blocks:
        in_ch = n_channels
        out_ch = n_outputs
        act = True
      else:
        in_ch = n_channels
        out_ch = n_channels
        act = True
      
      dilation = dilation_growth ** n
      self.blocks.append(TCNBlock(in_ch, out_ch, kernel_size, dilation, cond_dim=cond_dim, activation=act))

  def forward(self, x, c=None):
    #print("TCN::forward x:", x.shape, "c:", c)

    for block in self.blocks:
      x = block(x, c)
      #print("TCN::after block x:", x.shape)
      
    return x
  
  def compute_receptive_field(self):
    """Compute the receptive field in samples."""
    rf = self.kernel_size
    for n in range(1, self.n_blocks):
        dilation = self.dilation_growth ** (n % self.stack_size)
        rf = rf + ((self.kernel_size - 1) * dilation)
    return rf
