# Makefile — TP6 Gestion mémoire
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11
LDFLAGS=

all: ex1_malloc ex2_leak ex3_mmap

ex1_malloc: ex1_malloc.c
	$(CC) $(CFLAGS) $< -o $@

ex2_leak: ex2_leak.c
	$(CC) $(CFLAGS) $< -o $@

ex3_mmap: ex3_mmap.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ex1_malloc ex2_leak ex3_mmap
