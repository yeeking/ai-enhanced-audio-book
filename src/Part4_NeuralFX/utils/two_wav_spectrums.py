## Python script to plot the spectra of two WAV files next to eachother

import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.fft import fft
import sys

def plot_spectrum2(waveform, sample_rate, label, num_values_to_plot=100):
    # Compute the Fast Fourier Transform (FFT)
    spectrum = fft(waveform)
    
    # Calculate the corresponding frequencies
    frequencies = np.fft.fftfreq(len(waveform), d=1/sample_rate)
    
    # Take the absolute value and normalize the spectrum
    spectrum = np.abs(spectrum) / len(waveform)
    
    # Select evenly spaced indices to plot
    indices_to_plot = np.linspace(0, len(frequencies) // 2 - 1, num_values_to_plot, dtype=int)
    
    # Plot the spectrum
    plt.plot(frequencies[indices_to_plot], spectrum[indices_to_plot], label=label)


def plot_spectrum(waveform, sample_rate, label):
    # Compute the Fast Fourier Transform (FFT)
    spectrum = fft(waveform)
    
    # Calculate the corresponding frequencies
    frequencies = np.fft.fftfreq(len(waveform), d=1/sample_rate)
    
    # Take the absolute value and normalize the spectrum
    spectrum = np.abs(spectrum) / len(waveform)
    
    # Plot the spectrum
    plt.plot(frequencies[:len(frequencies)//2], spectrum[:len(spectrum)//2], label=label)

def main(wav_file_path1, wav_file_path2, img_file):
    # Replace 'path_to_audio_file1.wav' and 'path_to_audio_file2.wav' with the actual paths to your WAV files
    # wav_file_path1 = 'path_to_audio_file1.wav'
    # wav_file_path2 = 'path_to_audio_file2.wav'
    
    # Load the WAV files
    sample_rate1, waveform1 = wavfile.read(wav_file_path1)
    sample_rate2, waveform2 = wavfile.read(wav_file_path2)
    
    # Create a figure and plot both spectrums side by side
    plt.figure(figsize=(16, 6))
    plt.subplot(1, 2, 1)
    plot_spectrum2(waveform1, sample_rate1, 'Audio File 1')
    plt.title('Unfiltered drums')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.legend()
    plt.yscale('log')  # Set y-axis to logarithmic scale
    
    plt.subplot(1, 2, 2)
    plot_spectrum2(waveform2, sample_rate2, 'Audio File 2')
    plt.title('Drums with moving average 0.5,0.5,0.5 coefficients')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Amplitude')
    plt.grid(True)  
    plt.legend()

    plt.tight_layout()
    plt.yscale('log')  # Set y-axis to logarithmic scale
    #plt.show()
    plt.savefig(img_file)

if __name__ == '__main__':
    assert len(sys.argv) == 4, "meed 3 args: wav 1, wav 2 and plot img file"
    main(sys.argv[1], sys.argv[2], sys.argv[3])
