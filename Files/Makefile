
HOME=/home/jmw397
test:
	gcc threaded_cut.c -o threaded -L$(HOME)/lib/ -lsndfile -L../fftw3/ -l:libfftw3.so.3 -lm -lpthread -l:libfftw3_threads.so.3

run-test:
	LD_LIBRARY_PATH=$(HOME)/lib ./threaded online-dating.wav sample.wav
	rm threaded
