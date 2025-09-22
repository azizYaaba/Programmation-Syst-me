# Makefile — TP Fichiers et Permissions
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11

all: ex1_rw ex2_append ex3_stat

ex1_rw: ex1_rw.c
	$(CC) $(CFLAGS) $< -o $@

ex2_append: ex2_append.c
	$(CC) $(CFLAGS) $< -o $@

ex3_stat: ex3_stat.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ex1_rw ex2_append ex3_stat demo.txt demo_append.txt
