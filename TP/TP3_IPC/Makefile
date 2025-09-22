# Makefile — TP4 IPC
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11
LDFLAGS=
PTHREAD=-pthread

all: ex1_pipe ex2_sysv_msgq ex3_shm_sem

ex1_pipe: ex1_pipe.c
	$(CC) $(CFLAGS) $< -o $@

ex2_sysv_msgq: ex2_sysv_msgq.c
	$(CC) $(CFLAGS) $< -o $@

ex3_shm_sem: ex3_shm_sem.c
	$(CC) $(CFLAGS) $(PTHREAD) $< -o $@

clean:
	rm -f ex1_pipe ex2_sysv_msgq ex3_shm_sem
