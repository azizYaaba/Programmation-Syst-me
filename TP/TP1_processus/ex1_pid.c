// ex1_pid.c
// TP1 – Découverte des processus — Exercice 1
// Objectif : créer un processus fils et afficher les PID du père et du fils.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex1_pid.c -o ex1_pid
// Exécution   : ./ex1_pid
//
// Remarque : code POSIX (Linux/macOS). Sous Windows, utiliser WSL ou MinGW/Cygwin.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        // Code du fils
        printf("[FILS ] PID=%d, PPID=%d\n", getpid(), getppid());
    } else {
        // Code du père
        printf("[PERE ] PID=%d, fils=%d\n", getpid(), pid);
    }
    return EXIT_SUCCESS;
}
