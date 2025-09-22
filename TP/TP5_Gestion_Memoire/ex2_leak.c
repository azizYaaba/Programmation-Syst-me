// ex2_leak.c
// TP6 – Gestion mémoire — Exercice 2
// Objectif : provoquer une fuite mémoire volontaire, puis la détecter avec valgrind.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex2_leak.c -o ex2_leak
// Exécution   : ./ex2_leak
//
// Détection (Linux) : valgrind --leak-check=full ./ex2_leak
// Attendu : "definitely lost" > 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *p = malloc(1024);          // fuite : jamais libéré
    if (!p) { perror("malloc"); return 1; }
    strcpy(p, "Ceci est une fuite mémoire de 1 KiB.");
    printf("Programme termine (mais n'a pas libere son bloc !) -> verifier avec valgrind.\n");

    // Exemple additionnel : fuite multiple
    for (int i = 0; i < 10; ++i) {
        void *q = malloc(4096);      // 10 * 4 KiB -> 40 KiB
        if (!q) { perror("malloc"); return 1; }
        // perdu volontairement : pas de free(q);
    }

    return 0; // p et les 'q' ne sont pas libérés -> fuite
}
