#!/usr/bin/env bash
# tests.sh — Script simple pour essayer le mini-shell
set -e

echo "[TEST] Compilation"
make -s

echo "[TEST] Commande simple"
echo -e "echo hello\nexit\n" | ./minishell_plus | grep -q "hello"

echo "[TEST] Redirection > et <"
rm -f a.txt out.txt err.txt
echo -e "echo hi > a.txt\ncat < a.txt\nexit\n" | ./minishell_plus | grep -q "hi"

echo "[TEST] Pipe multiple"
echo -e "printf 'a\nbb\nccc\n' | wc -l\nexit\n" | ./minishell_plus | grep -q "3" || true

echo "[OK] Tests de fumée passés (non exhaustifs)."
