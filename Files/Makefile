
HOME=/home/jmw397
test:
	gcc final.c -o final -L$(HOME)/lib/ -lsndfile -L../fftw3/ -l:libfftw3.so.3 -lm -lpthread -l:libfftw3_threads.so.3

run-test:
	LD_LIBRARY_PATH=$(HOME)/lib ./final online-dating.wav sample.wav online-dating-trimmed.wav
	rm final
