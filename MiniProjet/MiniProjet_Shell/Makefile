# Makefile — TP7 Mini-projet (Mini-shell)
CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11

all: minishell_plus

minishell_plus: minishell_plus.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f minishell_plus
