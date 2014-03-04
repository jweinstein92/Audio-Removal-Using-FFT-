
HOME=/home/jmw397
test:
	gcc test.c -o test -L$(HOME)/lib/ -lsndfile

run-test:
	LD_LIBRARY_PATH=$(HOME)/lib ./test 2013-09-19-sysk-dying.wav
