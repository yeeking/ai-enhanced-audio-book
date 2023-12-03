import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

## this code uses the scipy signal package
## to compute IIR filter coefficients
## then it displays the frequency responses of the filters

order = 10

b1, a1 = signal.iirfilter(order, [2*np.pi*50, 2*np.pi*200], rs=60,rp=4,
                        btype='band', analog=True, ftype='ellip')

b2, a2 = signal.iirfilter(order, [2*np.pi*50, 2*np.pi*200], rs=60, rp=4,
                        btype='band', analog=True, ftype='cheby2')

w1, h1 = signal.freqs(b1, a1, 2000)
w2, h2 = signal.freqs(b2, a2, 2000)

#fig = plt.figure()

fig, axes = plt.subplots(ncols=1, nrows=2, figsize=(4, 5))


axes[0].semilogx(w1 / (2*np.pi), 20 * np.log10(np.maximum(abs(h1), 1e-5)))
#axes[0].set_xlabel('Frequency [Hz]')
axes[0].set_title('Elliptical order ' + str(order) )
axes[0].set_ylabel('Amplitude [dB]')
axes[0].axis((10, 1000, -100, 10))
axes[0].grid(which='both', axis='both')    
  
axes[1].semilogx(w2 / (2*np.pi), 20 * np.log10(np.maximum(abs(h2), 1e-5)))
axes[1].set_title('Chebyshev Type II order ' + str(order))
axes[1].set_xlabel('Frequency [Hz]')
axes[1].set_ylabel('Amplitude [dB]')
axes[1].axis((10, 1000, -100, 10))
axes[1].grid(which='both', axis='both') 

plt.tight_layout()

print(len(b1), len(a1))

plt.savefig('../../img/iir-freq-resp.pdf')
