#Audio Signal Removal Using Cross Correlation and FFT

##Setup
Install the libraries FFTW, http://www.fftw.org/ and libsndfile, http://www.mega-nerd.com/libsndfile/
Once installed, update the Makefile to represent where on your computer the .a or .so files for those libraries are located. Also, update the include statements in final.c

##Usage
In the same directory as the Makefile, place two WAV files. The first being the full audio file; the second being the chunk of audio you would like to remove from the first file. Currently, the program assumes the chunk is somewhere in the first file but only once. If the chunk is not in the first file, a random piece may be removed.

Update the Makefile to use the names of the two WAV files and the name you would like the output to be.

By default, the program uses eight threads to make the process faster. If your computer cannot handle eight threads, or it can do more, change line 115 of final.c - fftw_plan_with_nthreads(8) - with however many threads you would like to use. The program still runs well using only one thread.

###Credits###
This program was written and tested by Josh Weinstein and Tracey Young of Drexel University, March 2014.

A special thanks to those who helped with various aspects of the project including checking the math involved with the project and helping setting up the necessary libraries.
* Eric Hartman
* Brandon Katz
* Louis Kratz
