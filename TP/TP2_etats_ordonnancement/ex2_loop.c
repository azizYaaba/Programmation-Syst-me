// ex2_loop.c
// TP2 – États des processus et ordonnancement — Exercice 2
// Programme qui tourne en boucle et affiche périodiquement un "heartbeat".
// Objectif : envoyer des signaux depuis un autre terminal :
//   kill -STOP <PID>  -> le processus passe en état T (Stopped)
//   kill -CONT <PID>  -> reprend (R/S)
//   kill -KILL <PID>  -> termine immédiatement
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex2_loop.c -o ex2_loop
// Exécution   : ./ex2_loop

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static volatile sig_atomic_t running = 1;

static void sigterm_handler(int sig) {
    (void)sig;
    fprintf(stderr, "[PROC] Reçu SIGTERM, sortie propre.\n");
    running = 0;
}

static void sigint_handler(int sig) {
    (void)sig;
    fprintf(stderr, "[PROC] Reçu SIGINT (Ctrl-C), sortie propre.\n");
    running = 0;
}

int main(void) {
    fprintf(stderr, "[PROC] PID=%d — envoyez des signaux (STOP/CONT/KILL) depuis un autre terminal.\n", getpid());

    // NB: SIGSTOP et SIGKILL ne peuvent PAS être interceptés/ignorés/traités.
    // On installe des handlers sur SIGTERM/SIGINT pour montrer des sorties "propres".
    signal(SIGTERM, sigterm_handler);
    signal(SIGINT,  sigint_handler);

    unsigned long long i = 0;
    while (running) {
        if ((i++ % 100000000ULL) == 0ULL) {
            fprintf(stderr, "[PROC] vivant... (i=%llu)\n", i);
            // dormir un peu pour rendre l'observation plus simple
            usleep(200000); // 200ms
        }
    }
    fprintf(stderr, "[PROC] Fin normale. Bye.\n");
    return 0;
}
