#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../libsndfile/src/sndfile.h"
#include "../fftw3/api/fftw3.h"


int main(int argc, char *argv[]) {
	printf("Wav Read Test\n");
	if (argc != 4) {
		printf("Expected usage: ./threaded_cut source-file.wav sample-file.wav outputName\n");
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
	double *main_array = malloc(mainInfo.frames*sizeof(double));
	if (main_array == NULL) {
		printf("Could not allocate memory for data\n");
		sf_close(mainFile);
		return 1;
	}

	//Load data
	double main_numFrames = sf_readf_double(mainFile, main_array, mainInfo.frames);

	//Check correct number of samples loaded
	if (main_numFrames != mainInfo.frames) {
		printf("Did not read enough frames for source\n");
		sf_close(mainFile);
		free(main_array);
		return 1;
	}

	//Output info
	printf("Read %f frames from %s, Sample rate: %d, Length: %fs\n",
    	main_numFrames, argv[1], mainInfo.samplerate, (double)main_numFrames/mainInfo.samplerate);
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
	double *sample_array = malloc(sampleInfo.frames * sizeof(double));
	if (sample_array == NULL) {
		printf("Could not allocate memory for sample data\n");
		sf_close(sampleFile);
		free(main_array);
		return 1;
	}

	//Load data
	double sample_numFrames = sf_readf_double(sampleFile, sample_array,
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
	(double)sample_numFrames/sampleInfo.samplerate);
	sf_close(sampleFile);

	//For threaded FFTW
	fftw_init_threads();
	fftw_plan_with_nthreads(8);
	
	//Correlated Length
	int corrLength = mainInfo.frames - sampleInfo.frames + 1;
	int lenCon = pow(2, ceil(log2(mainInfo.frames + sampleInfo.frames - 1)));

	//Calculate the cross correlation.
	printf("Calculate cross correlation\n");
	double* x_pad = malloc(lenCon * sizeof(double));
	memset(x_pad, 0, lenCon * sizeof(double));
	double* y_pad = malloc(lenCon * sizeof(double));
	memset(y_pad, 0, lenCon * sizeof(double));

	int i;
	for (i = 0; i < mainInfo.frames; i++) {
		x_pad[i] = main_array[i];
	}

	for (i = 0; i < sampleInfo.frames; i++) {
		y_pad[i] = sample_array[i];
	}
	
	fftw_complex* X = (fftw_complex*)fftw_malloc(lenCon * sizeof(fftw_complex));
	memset(X, 0, lenCon * sizeof(fftw_complex));
	fftw_plan plan3 = fftw_plan_dft_r2c_1d(lenCon, x_pad, X, FFTW_ESTIMATE);
	fftw_execute(plan3);
	fftw_destroy_plan(plan3);

	fftw_complex* Y = (fftw_complex*)fftw_malloc(lenCon * sizeof(fftw_complex));
	memset(Y, 0, lenCon * sizeof(fftw_complex));
	fftw_plan plan4 = fftw_plan_dft_r2c_1d(lenCon, y_pad, Y, FFTW_ESTIMATE);
	fftw_execute(plan4);
	fftw_destroy_plan(plan4);
	
	//Take the complex conjugate of the complex array Y
	printf("Calculating the complex conjugate Y\n");
	fftw_complex* yConjugate = (fftw_complex*)fftw_malloc(lenCon * sizeof(fftw_complex));
	memset(yConjugate, 0, lenCon * sizeof(fftw_complex));
	for (i = 0; i < lenCon; i++) {
		yConjugate[i][0] = Y[i][0];
		yConjugate[i][1] = (-1) * Y[i][1];
	}

	//Array multiplication between the FFT of X and conjugate of FFT Y.
	printf("Complex array multiplication X yConjugate\n");
	fftw_complex* corrArray = (fftw_complex*)fftw_malloc(lenCon * sizeof(fftw_complex));
	memset(corrArray, 0, lenCon * sizeof(fftw_complex));
	int j;
	for (j = 0; j < lenCon; j++) {
		corrArray[j][0] = X[j][0] * yConjugate[j][0] - X[j][1] * yConjugate[j][1];
		corrArray[j][1] = X[j][0] * yConjugate[j][1] + X[j][1] * yConjugate[j][0];
	}

	//Take the inverse FFT and store in corr.
	double* corr = malloc(lenCon * sizeof(double));
	memset(corr, 0, lenCon * sizeof(double));
	printf("Calculating inverse FFT corr\n");
	fftw_plan plan5 = fftw_plan_dft_c2r_1d(lenCon, corrArray, corr, FFTW_ESTIMATE);
	fftw_execute(plan5);
	fftw_destroy_plan(plan5);

	//Round values of corr.
	for (j = 0; j < corrLength; j++) {
		corr[j] = round(corr[j]);
	}

	int match = -1;
	double max = -INFINITY;
	printf("Determining location of closest match\n");
	for (j = 0; j < lenCon; j++) {
		if (corr[j] > max) {
			match = j;
			max = corr[j];
		}
	}
	printf("Match Index: %d\n", match);
	printf("Max: %lf\n", max);

	//Remove matched signal
	int size = mainInfo.frames - sampleInfo.frames;
	double* new_sig = malloc(size * sizeof(double));
	memset(new_sig, 0, size * sizeof(double));
	printf("Creating new signal\n");
	j = 0;
	int range = match + sampleInfo.frames;
	for (i = 0; i < mainInfo.frames; i++) {
		if (i < match || i > range) {
			new_sig[j] = main_array[i];
			j = j + 1;
		}
	}


	free(main_array);
	free(sample_array);
	free(x_pad);
	free(y_pad);
	free(corr);
	fftw_cleanup_threads();

	//Write the data
	printf("Writing copy\n");
	SNDFILE* outfile;
	SF_INFO tmp;
	tmp.format = mainInfo.format;
	tmp.channels = mainInfo.channels;
	tmp.samplerate = mainInfo.samplerate;
	
	outfile = sf_open(argv[3], SFM_WRITE, &tmp);
	if (outfile == NULL) {
		printf("Error opening output file\n");
		return 1;
	}

	// Write frames
	printf("Writing data\n");
	long writtenFrames = sf_writef_double(outfile, new_sig, size);

	if (writtenFrames != size) {
		printf("Did not write enough frames to output file.\n");
		sf_close(outfile);
		free(new_sig);
		return 1;
	}

	sf_write_sync(outfile);
	sf_close(outfile);

	free(new_sig);
	fftw_cleanup();

	return 0;
}
