// ex2_mutex.c
// TP5 – Synchronisation — Exercice 2
// Objectif : corriger la course avec un mutex pthread.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex2_mutex.c -o ex2_mutex -pthread
// Exécution   : ./ex2_mutex [threads] [iters]
//
// Exemple : ./ex2_mutex 8 2000000
// Attendu : résultat EXACT (threads*iters).

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

static long long counter = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int id;
    long long iters;
} Task;

void* worker(void* arg) {
    Task *t = (Task*)arg;
    for (long long i = 0; i < t->iters; ++i) {
        pthread_mutex_lock(&mtx);
        counter++;
        pthread_mutex_unlock(&mtx);
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
    if (counter == expected) {
        printf(">>> Succès : la section critique protégée par mutex empêche la course\n");
    } else {
        printf(">>> Échec inattendu (vérifiez la logique)\n");
    }

    return 0;
}
