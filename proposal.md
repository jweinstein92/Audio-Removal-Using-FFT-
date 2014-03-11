#Audio Signal Removal Using Cross Correlation and FFT

##Description
Given an input signal and a sampled signal in the form of WAV files, the program extracts the stored data of the files.

With the assumption that there is a single match in the input signal, the sampled signal can be found by computing the cross-correlation of the two signals. Because cross correlation is defined as summation over l of the point-by-point multiplication of the overlapping signals at each offset, l, from negative infinity to infinity, (or until the signals no longer overlap), this calculation is untractable.

Noting that cross correlation is convolution with a signal pre-flipped (or unflipped), the cross correlation of the two signals can be more easily found by taking the discrete, or fast, Fourier transform of the two signals, with zero-padding to ensure that the entire signal is convolved, and applying the property of duality and multiplying the transformed signal of one by the complex conjugate of the transformed signal of the other. This calculation results in the transformed cross correlation signal, and the cross correlation signal in time can be retrieved by taking the inverse fast Fourier transform. 

The matching signal in the input signal can then be found by locating the cross correlation offset in the cross correlation signal that is highest.

After locating the matching signal, it can then removed from the input signal.

##Motivation
The motivation for this project is to provide a fast, simple way to remove chunks of an audio file with two provided WAV files, one for the original signal and one for the sample. Current systems allow the user to remove aspects of a WAV file based on a time frame. However, it is not always desirable to search through a given file, looking for the chunk one wants to remove. Especially if there are multiple files with the same sound being removed or there is a chance of the sound being used in multiple places throughout the audio. While the proposed system will not be able to handle multiple occurrences of the sample file, it will be able to work on files with the same sample input, making removal of the sample for all of them faster.

This project will be a simple command line program only requiring two files. There will not be any extra visuals that are not necessary for the removal of signals.

##Objectives
Successfully calculate the cross correlation with the provided signals broken down into arrays of floating point integers. We will attempt to perform these calculations using the Fast Fourier Transform however if we are unable to do so, we will calculate the correlations based on the definitions of them.

Successfully match the sample signal inside the main signal by utilizing the cross correlated signal. Using the results of the correlations, choosing the result with the highest value will be considered the signal match.

Successfully remove the sample and create a new signal with the sample removed.
