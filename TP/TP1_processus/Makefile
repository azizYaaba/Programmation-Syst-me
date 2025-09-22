# Makefile pour TP1 – Découverte des processus
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11

all: ex1_pid ex2_exec_ls ex3_wait ex_all

ex1_pid: ex1_pid.c
	$(CC) $(CFLAGS) $< -o $@

ex2_exec_ls: ex2_exec_ls.c
	$(CC) $(CFLAGS) $< -o $@

ex3_wait: ex3_wait.c
	$(CC) $(CFLAGS) $< -o $@

ex_all: ex_all.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ex1_pid ex2_exec_ls ex3_wait ex_all
