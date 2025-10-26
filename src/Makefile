CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic -std=gnu99 -pthread
EXAMPLES=fibs fauxgrep fauxgrep-mt fhistogram fhistogram-mt
TESTS=Test_files/FG_test Test_files/FH_test Test_files/JQ_test Test_files/Time_test

.PHONY: all test clean ../src.zip

all: $(TESTS) $(EXAMPLES)

job_queue.o: job_queue.c job_queue.h
	$(CC) -c job_queue.c $(CFLAGS)

%: %.c job_queue.o
	$(CC) -o $@ $^ $(CFLAGS)

test: $(TESTS)
	@set e; for test in $(TESTS); do echo ./$$test; ./$$test; done

clean:
	rm -rf $(TESTS) $(EXAMPLES) *.o core
	rm -rf Test_files/Inputs/test1.txt Test_files/Inputs/test2.txt Test_files/Inputs/test3.txt
	rm -rf Test_files/Inputs/Tgrep1.txt Test_files/Inputs/Tgrep2.txt Test_files/Inputs/Tgrep3.txt
	rm -rf Test_files/Inputs/speedfhtest.txt Test_files/Inputs/speedfgtest.txt

zip: ../src.zip

../src.zip:
	make clean
	cd .. && zip src.zip -r src
