// ex1_malloc.c
// TP6 – Gestion mémoire — Exercice 1
// Objectif : allouer un tableau dynamique avec malloc, le remplir, l'afficher puis le libérer.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex1_malloc.c -o ex1_malloc
// Exécution   : ./ex1_malloc [n]
//   n = taille du tableau (défaut 10)

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    long n = (argc > 1) ? strtol(argv[1], NULL, 10) : 10;
    if (n <= 0 || n > 100000000) {
        fprintf(stderr, "Taille invalide (1..1e8)\n");
        return 1;
    }

    int *a = malloc((size_t)n * sizeof(int));
    if (!a) {
        perror("malloc");
        return 1;
    }

    for (long i = 0; i < n; ++i) a[i] = (int)(i * 2);
    printf("Tableau (%ld elements):\n", n);
    long show = n < 50 ? n : 50; // éviter d'inonder la sortie
    for (long i = 0; i < show; ++i) printf("%d ", a[i]);
    if (n > show) printf("... (affiche les 50 premiers)");
    printf("\n");

    free(a);
    return 0;
}
