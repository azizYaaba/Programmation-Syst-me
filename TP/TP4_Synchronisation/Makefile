# Makefile — TP5 Synchronisation
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11
LDFLAGS=-pthread

all: ex1_race ex2_mutex ex3_prodcons

ex1_race: ex1_race.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

ex2_mutex: ex2_mutex.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

ex3_prodcons: ex3_prodcons.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f ex1_race ex2_mutex ex3_prodcons
