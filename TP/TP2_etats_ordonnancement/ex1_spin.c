// ex1_spin.c
// TP2 – États des processus et ordonnancement — Exercice 1
// Lancer un processus fils qui boucle infiniment pour observer son état avec ps/top.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex1_spin.c -o ex1_spin
// Exécution   : ./ex1_spin [sleep]
//   - sans argument : boucle CPU intensive (R dans ps/top)
//   - avec "sleep"  : boucle avec sleep(1) (état S la plupart du temps)
//
// Observation :
//   - ps -o pid,ppid,stat,ni,pri,cmd -p <PID_FILS>
//   - top ou htop : voir %CPU, état (R/S/T), priorité, nice.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

static void busy_loop(void) {
    volatile unsigned long long x = 0;
    while (1) {
        x += 1; // empêche l'optimisation
        if ((x & ((1ULL<<26)-1)) == 0) {
            // "heartbeat" occasionnel
            fprintf(stderr, "[FILS] PID=%d toujours actif (busy)\n", getpid());
        }
    }
}

static void sleepy_loop(void) {
    while (1) {
        fprintf(stderr, "[FILS] PID=%d en sommeil...\n", getpid());
        sleep(1);
    }
}

int main(int argc, char **argv) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        // FILS
        fprintf(stderr, "[FILS] PID=%d, PPID=%d\n", getpid(), getppid());
        if (argc > 1 && argv[1] && strcmp(argv[1], "sleep") == 0) {
            sleepy_loop();
        } else {
            busy_loop();
        }
        return 0;
    } else {
        // PERE
        fprintf(stderr, "[PERE] PID=%d, fils=%d — utilisez ps/top pour observer.\n", getpid(), pid);
        fprintf(stderr, "[PERE] Astuce : 'ps -o pid,ppid,stat,ni,pri,cmd -p %d'\n", pid);
        // Le père ne se termine pas tout de suite pour garder un PPID valide au fils
        // mais on n'attend pas : ce n'est PAS un zombie, car le fils vit.
        // Pour garder le TP simple, le père dort un long moment.
        sleep(3600);
    }
    return 0;
}
