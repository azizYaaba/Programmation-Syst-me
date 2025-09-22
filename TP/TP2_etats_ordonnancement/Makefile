# Makefile — TP2 États des processus et ordonnancement
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11

all: ex1_spin ex2_loop ex2_controller

ex1_spin: ex1_spin.c
	$(CC) $(CFLAGS) $< -o $@

ex2_loop: ex2_loop.c
	$(CC) $(CFLAGS) $< -o $@

ex2_controller: ex2_controller.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ex1_spin ex2_loop ex2_controller
