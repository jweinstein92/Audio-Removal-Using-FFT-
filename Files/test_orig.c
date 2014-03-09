#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../libsndfile/src/sndfile.h"
#include "../fftw/fftw/fftw.h"

int main(int argc, char *argv[]) {
	printf("Wav Read Test\n");
	if (argc != 3) {
		printf("Expected usage: ./test source-file.wav sample-file.wav\n");
		return 1;
	}

	//Open sound file
	SF_INFO mainInfo;;
	SNDFILE *mainFile = sf_open(argv[1], SFM_READ, &mainInfo);
	if (mainFile == NULL) {
		printf("Error reading source file '%s': %s\n", argv[1], sf_strerror(mainFile));
		return 1;
	}

	//Check format - 16bit PCM
	if (mainInfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		printf("Input should be 16bit Wav\n");
		sf_close(mainFile);
		return 1;
	}

	//Check channels - mono
	if (mainInfo.channels != 1) {
		printf("Wrong number of channels\n");
		sf_close(mainFile);
		return 1;
	}

	//Allocate memory
	float *main_array = malloc(mainInfo.frames*sizeof(float));
	if (main_array == NULL) {
		printf("Could not allocate memory for data\n");
		sf_close(mainFile);
		return 1;
	}

	//Load data
	float main_numFrames = sf_readf_float(mainFile, main_array, mainInfo.frames);

	//Check correct number of samples loaded
	if (main_numFrames != mainInfo.frames) {
		printf("Did not read enough frames for source\n");
		sf_close(mainFile);
		free(main_array);
		return 1;
	}

	//Output info
	printf("Read %f frames from %s, Sample rate: %d, Length: %fs\n",
    	main_numFrames, argv[1], mainInfo.samplerate, (float)main_numFrames/mainInfo.samplerate);
	sf_close(mainFile);
 
	
	//Sample file
	SF_INFO sampleInfo;
	SNDFILE *sampleFile = sf_open(argv[2], SFM_READ, &sampleInfo);
	if (sampleFile == NULL) {
		printf("Error reading sample file '%s': %s\n", argv[2], sf_strerror(sampleFile));
		free(main_array);
		return 1;
	}

	//Check format - 16bit PCM
	if (sampleInfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		printf("Input should be 16bit WAV\n");
		sf_close(sampleFile);
		free(main_array);
		return 1;
	}

	//Check channels - mono
	if (sampleInfo.channels != 1) {
		printf("Wrong number of channels - sample\n");
		sf_close(sampleFile);
		free(main_array);
		return 1;
	}

	//Allocate memory
	float *sample_array = malloc(sampleInfo.frames * sizeof(float));
	if (sample_array == NULL) {
		printf("Could not allocate memory for sample data\n");
		sf_close(sampleFile);
		free(main_array);
		return 1;
	}

	//Load data
	float sample_numFrames = sf_readf_float(sampleFile, sample_array,
	sampleInfo.frames);

	//Check correct nuber of frames loaded
	if (sample_numFrames != sampleInfo.frames) {
		printf("Did not read enough frames for sample\n");
		sf_close(sampleFile);
		free(main_array);
		free(sample_array);
		return 1;
	}

	//Output info
	printf("Read %f frames from %s, Sample rate: %d, Length: %fs\n",
	sample_numFrames, argv[2], sampleInfo.samplerate,
	(float)sample_numFrames/sampleInfo.samplerate);
	sf_close(sampleFile);

	
	
	//Correlated Length
	int corrLength = mainInfo.frames - sampleInfo.frames + 1;
	int lenCon = pow(2, ceil(log2(mainInfo.frames + sampleInfo.frames - 1)));

	//Calculate rxx0 values
	float* rxx = malloc(lenCon * sizeof(float));
	float* rxx0 = malloc(corrLength * sizeof(float));
	memset(rxx, 0, lenCon * sizeof(float));
	memset(rxx0, 0, corrLength * sizeof(float));

	// subSection represents x(i:(i+length(y)-1)).
	float* subSection = malloc(sampleInfo.frames * sizeof(float));
	// nPoint represents 2*length(x) - 1).
	int nPoint = 2*mainInfo.frames - 1;
	int i;
	for (i = 0; i < corrLength; i++) {
		memcpy(subSection, main_array + i, sampleInfo.frames*sizeof(float));
		// fftw_1 represents fft(x(i:(i+length(y)-1).
		printf("Calculating FFT\n");
		fftw_complex* fftw_1 = (fftw_complex*)fftw_malloc(nPoint * sizeof(fftw_complex));	
		memset(fftw_1, 0, nPoint * sizeof(fftw_complex));
		fftw_plan plan1 = fftw_plan_dft_r2c_1d(nPoint, subSection, fftw_1, FFTW_MEASURE);
		fftw_execute(plan1);
		fftw_destroy(plan1);
		// fftw_1 now contains the complex data from running fftw on subSection.

		// fftw_2 represents fft(x(i:(i+length(y)-1).
		//fftw_complex* fftw_2 = (fftw_complex*)fftw_malloc(nPoint * sizeof(fftw_complex));
		//memset(fftw_2, 0, nPoint * sizeof(fftw_complex));
		//fftw_plan plan2 = fftw_plan_dft_r2c_1d(nPoint, fftw_2, subSection, FFTW_MEASURE);
		//fftw_execute(plan2);
		// fftw_2 now contains the complex data from running fftw on subSection.
	

		// Take the complex conjugate of the complex array fftw_1
		printf("Calculating the complex conjugate\n");
		fftw_complex* conjugate = (fftw_complex*)fftw_malloc(nPoint * sizeof(fftw_complex));
		memset(conjugate, 0, nPoint * sizeof(fftw_complex));
		int j;
		for (j = 0; j < nPoint; j++) {
			//conjugate[j] = conj(fftw_2[j]);
			conjugate[j][0] = fftw_1[j][0];
			conjugate[j][1] = (-1) * fftw_1[j][1];
		}
		
		//Array multiplication between the FFT and conjugate of FFT.
		printf("Complex array multiplication\n");
		fftw_complex* final = (fftw_complex*)fftw_malloc(nPoint * sizeof(fftw_complex));
		memset(final, 0, nPoint * sizeof(fftw_complex));
		for (j = 0; j < nPoint; j++) {
			final[j][0] = fftw_1[j][0] * conjugate[j][0] - fftw_1[j][1] * conjugate[j][1];
			final[j][1] = fftw_1[j][0] * conjugate[j][1] + fftw_1[j][1] * conjugate[j][0];
		}

		//Take the inverse FFT and store in rxx.
		printf("Calculating inverse FFT\n");
		fftw_plan plan3 = fftw_plan_dft_c2r_1d(nPoint, final, rxx, FFTW_MEASURE);
		fftw_execute(plan3);
		fftw_destroy(plan3);
		rxx0[i] = round(rxx[0]);
	}
	
	printf("Calculate normalized cross correlation\n");
	float x_pad[lenCon];
	memset(x_pad, 0, lenCon*sizeof(float));
	float y_pad[lenCon];
	memset(y_pad, 0, lenCon*sizeof(float));

	for (i = 0; i < mainInfo.frames; i++) {
		x_pad[i] = main_array[i];
	}
	
	
	
	
	
	
	free(main_array);
	free(sample_array);


	//float_array is the array of floating point numbers needed to be used.


	//Write the data
	//printf("Writing copy\n");
	//SNDFILE* outfile;
	//SF_INFO tmp;
	//tmp.samplerate = sndInfo.samplerate;

	//Write the data
	//printf("Writing copy\n");
	//SNDFILE* outfile;
	//SF_INFO tmp;
	//tmp.samplerate = sndInfo.samplerate;

