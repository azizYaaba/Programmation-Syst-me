# Makefile — TP Multithreading (pthreads) V2
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11
LDFLAGS=-pthread

all: ex1_threads_basics ex2_race ex3_mutex ex4_prodcons ex5_barrier ex6_deadlock ex7_starvation ex8_condvar

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f ex1_threads_basics ex2_race ex3_mutex ex4_prodcons ex5_barrier ex6_deadlock ex7_starvation ex8_condvar
