# Makefile — Mini-projet Parallel Grep
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11
LDFLAGS=-pthread

all: pgrep_parallel

pgrep_parallel: src/pgrep_parallel.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f pgrep_parallel results.csv
