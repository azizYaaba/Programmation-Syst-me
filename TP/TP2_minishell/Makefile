# Makefile — TP2 Mini-Shell
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11

all: minishell

minishell: minishell.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f minishell
