
HOME=/home/jmw397
test:
	gcc test.c -o test -L$(HOME)/lib/ -lsndfile -L../fftw3/ -l:libfftw3.so.3 -lm

run-test:
	LD_LIBRARY_PATH=$(HOME)/lib ./test smaller.wav sample.wav
	rm test
