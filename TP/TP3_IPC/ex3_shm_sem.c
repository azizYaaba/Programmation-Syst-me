// ex3_shm_sem.c
// TP4 – IPC — Exercice 3: Mémoire partagée + sémaphore (POSIX, non-nommés)
// Objectif: partager un entier entre père et fils et l'incrémenter en section critique.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex3_shm_sem.c -o ex3_shm_sem -pthread
// Exécution   : ./ex3_shm_sem
//
// Remarque: nécessite Linux/WSL ou un POSIX supportant sem_init(pshared=1).

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <errno.h>

typedef struct Shared {
    sem_t mutex;      // sémaphore de mutualisation
    int   counter;    // variable partagée
} Shared;

int main(void) {
    // Créer un segment mémoire partagé anonyme
    Shared *sh = mmap(NULL, sizeof(Shared),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sh == MAP_FAILED) { perror("mmap"); return 1; }

    // Initialiser le sémaphore: pshared=1 (entre processus), valeur initiale 1 (mutex binaire)
    if (sem_init(&sh->mutex, 1, 1) != 0) { perror("sem_init"); return 1; }
    sh->counter = 0;

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    int iters = 100000; // augmenter pour voir l'intérêt de la synchro
    if (pid == 0) {
        // --- FILS ---
        for (int i = 0; i < iters; ++i) {
            if (sem_wait(&sh->mutex) != 0) { perror("sem_wait"); _exit(1); }
            sh->counter += 1;
            if (sem_post(&sh->mutex) != 0) { perror("sem_post"); _exit(1); }
        }
        _exit(0);
    } else {
        // --- PERE ---
        for (int i = 0; i < iters; ++i) {
            if (sem_wait(&sh->mutex) != 0) { perror("sem_wait"); return 1; }
            sh->counter += 1;
            if (sem_post(&sh->mutex) != 0) { perror("sem_post"); return 1; }
        }
        int status=0;
        waitpid(pid, &status, 0);

        // Attendu: counter == 2*iters
        printf("[PERE] Compteur final attendu=%d, obtenu=%d\n", iters*2, sh->counter);

        // Nettoyage
        if (sem_destroy(&sh->mutex) != 0) { perror("sem_destroy"); }
        if (munmap(sh, sizeof(Shared)) != 0) { perror("munmap"); }
    }
    return 0;
}
