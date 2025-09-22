// ex3_prodcons.c
// TP5 – Synchronisation — Exercice 3
// Problème producteur/consommateur avec tampon borné, utilisant semaphores POSIX (non nommés) + mutex.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex3_prodcons.c -o ex3_prodcons -pthread
// Exécution   : ./ex3_prodcons [nb_producers] [nb_consumers] [items_per_prod]
//
// Exemple : ./ex3_prodcons 2 2 50
//
// Tampon circulaire :
//  - sem_vide  = nombre d'emplacements libres
//  - sem_plein = nombre d'éléments présents
//  - mutex     = exclusion mutuelle sur l'accès au tampon

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#ifndef BUF_SIZE
#define BUF_SIZE 16
#endif

typedef int item_t;

typedef struct {
    item_t buf[BUF_SIZE];
    int head;
    int tail;
    sem_t sem_vide;
    sem_t sem_plein;
    pthread_mutex_t mutex;
} BQueue;

static BQueue q;

static void bq_init(BQueue *q) {
    q->head = q->tail = 0;
    if (sem_init(&q->sem_vide, 0, BUF_SIZE) != 0) { perror("sem_init(vide)"); exit(1); }
    if (sem_init(&q->sem_plein, 0, 0) != 0) { perror("sem_init(plein)"); exit(1); }
    if (pthread_mutex_init(&q->mutex, NULL) != 0) { perror("pthread_mutex_init"); exit(1); }
}

static void bq_destroy(BQueue *q) {
    sem_destroy(&q->sem_vide);
    sem_destroy(&q->sem_plein);
    pthread_mutex_destroy(&q->mutex);
}

static void bq_push(BQueue *q, item_t x) {
    sem_wait(&q->sem_vide);
    pthread_mutex_lock(&q->mutex);
    q->buf[q->tail] = x;
    q->tail = (q->tail + 1) % BUF_SIZE;
    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->sem_plein);
}

static item_t bq_pop(BQueue *q) {
    sem_wait(&q->sem_plein);
    pthread_mutex_lock(&q->mutex);
    item_t x = q->buf[q->head];
    q->head = (q->head + 1) % BUF_SIZE;
    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->sem_vide);
    return x;
}

typedef struct {
    int id;
    int items;
} ProdArg;

typedef struct {
    int id;
    int total_to_consume;
} ConsArg;

void* producer(void* arg) {
    ProdArg *p = (ProdArg*)arg;
    for (int i = 1; i <= p->items; ++i) {
        int val = p->id*100000 + i;
        bq_push(&q, val);
        fprintf(stderr, "[PROD %d] -> %d\n", p->id, val);
        // optionnel : simulation de travail
        // usleep(10000);
    }
    return NULL;
}

void* consumer(void* arg) {
    ConsArg *c = (ConsArg*)arg;
    for (int i = 0; i < c->total_to_consume; ++i) {
        int val = bq_pop(&q);
        fprintf(stderr, "             [CONS %d] <- %d\n", c->id, val);
        // optionnel : usleep(15000);
    }
    return NULL;
}

int main(int argc, char **argv) {
    int NP = (argc > 1) ? atoi(argv[1]) : 2;
    int NC = (argc > 2) ? atoi(argv[2]) : 2;
    int ITEMS = (argc > 3) ? atoi(argv[3]) : 50;

    if (NP <= 0 || NC <= 0 || ITEMS <= 0) {
        fprintf(stderr, "Usage: %s [nb_producers>0] [nb_consumers>0] [items_per_prod>0]\n", argv[0]);
        return 1;
    }

    bq_init(&q);

    pthread_t *tp = malloc(sizeof(pthread_t)*NP);
    pthread_t *tc = malloc(sizeof(pthread_t)*NC);
    ProdArg *pargs = malloc(sizeof(ProdArg)*NP);
    ConsArg *cargs = malloc(sizeof(ConsArg)*NC);

    // Chaque producteur produit ITEMS éléments => total NP*ITEMS
    // Chaque consommateur consomme (NP*ITEMS)/NC (on suppose divisible ; sinon dernier consomme le reste)
    int total = NP * ITEMS;
    int base = total / NC;
    int rest = total % NC;

    for (int i = 0; i < NP; ++i) {
        pargs[i].id = i;
        pargs[i].items = ITEMS;
        if (pthread_create(&tp[i], NULL, producer, &pargs[i]) != 0) { perror("pthread_create producer"); return 1; }
    }
    for (int i = 0; i < NC; ++i) {
        cargs[i].id = i;
        cargs[i].total_to_consume = base + (i == NC-1 ? rest : 0);
        if (pthread_create(&tc[i], NULL, consumer, &cargs[i]) != 0) { perror("pthread_create consumer"); return 1; }
    }

    for (int i = 0; i < NP; ++i) pthread_join(tp[i], NULL);
    for (int i = 0; i < NC; ++i) pthread_join(tc[i], NULL);

    bq_destroy(&q);
    free(tp); free(tc); free(pargs); free(cargs);

    fprintf(stderr, "Terminé. Produit=%d, Consommé=%d\n", total, total);
    return 0;
}
