// ex1_race.c
// TP5 – Synchronisation — Exercice 1
// Objectif : montrer une condition de course en incrémentant une variable globale sans protection.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex1_race.c -o ex1_race -pthread
// Exécution   : ./ex1_race [threads] [iters]
//
// Exemple : ./ex1_race 8 2000000
// Attendu : résultat INFERIEUR à threads*iters (souvent).

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

static volatile long long counter = 0;

typedef struct {
    int id;
    long long iters;
} Task;

void* worker(void* arg) {
    Task *t = (Task*)arg;
    for (long long i = 0; i < t->iters; ++i) {
        // SECTION NON PROTÉGÉE -> condition de course
        counter++; // non atomique
    }
    return NULL;
}

int main(int argc, char** argv) {
    int nthreads = (argc > 1) ? atoi(argv[1]) : 4;
    long long iters = (argc > 2) ? atoll(argv[2]) : 1000000;

    pthread_t *th = malloc(sizeof(pthread_t)*nthreads);
    Task *tasks = malloc(sizeof(Task)*nthreads);

    for (int i = 0; i < nthreads; ++i) {
        tasks[i].id = i;
        tasks[i].iters = iters;
        if (pthread_create(&th[i], NULL, worker, &tasks[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }
    for (int i = 0; i < nthreads; ++i) pthread_join(th[i], NULL);

    long long expected = (long long)nthreads * iters;
    printf("Attendu : %lld\n", expected);
    printf("Obtenu  : %lld\n", counter);
    if (counter != expected) {
        printf(">>> Condition de course observée (perte d'incréments)\n");
    } else {
        printf(">>> Pas de perte observée (chance ou CPU) — réessayez avec plus d'itérations\n");
    }

    free(th);
    free(tasks);
    return 0;
}
