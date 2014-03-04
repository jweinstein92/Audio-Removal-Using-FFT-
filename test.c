#include <stdio.h>
#include <stdlib.h>
#include "libsndfile/src/sndfile.h"

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

	free(main_array);
	free(sample_array);


	//float_array is the array of floating point numbers needed to be used.


	//Write the data
	//printf("Writing copy\n");
	//SNDFILE* outfile;
	//SF_INFO tmp;
	//tmp.samplerate = sndInfo.samplerate;
	//tmp.channels = sndInfo.channels;
	//tmp.format = sndInfo.format;

	//Open output file
	//outfile = sf_open("output.wav", SFM_WRITE,  &tmp);
	//if (outfile == NULL) {
		//printf("Not able to open output file\n");
		//sf_perror(NULL);
		//return 1;
	//}
	//sf_write_float(outfile, buffer, sndInfo.frames);
	
	//free(buffer);

   return 0;
}
