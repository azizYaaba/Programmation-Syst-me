# Makefile — TP Benchmarking
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11

all: ex1_naive ex2_buffered ex3_mmap

ex1_naive: ex1_naive.c
	$(CC) $(CFLAGS) $< -o $@

ex2_buffered: ex2_buffered.c
	$(CC) $(CFLAGS) $< -o $@

ex3_mmap: ex3_mmap.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ex1_naive ex2_buffered ex3_mmap bigfile.bin
