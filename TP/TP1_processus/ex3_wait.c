// ex3_wait.c
// TP1 – Découverte des processus — Exercice 3
// Objectif : le père attend la fin du fils (waitpid) et récupère le code de retour.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex3_wait.c -o ex3_wait
// Exécution   : ./ex3_wait
//
// Astuce : tester différents retours du fils (EXIT_SUCCESS/EXIT_FAILURE).

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        // Fils : exécute une commande, puis termine
        execlp("ls", "ls", "-l", (char*)NULL);
        perror("execlp(ls)"); // seulement si échec
        _exit(127);
    } else {
        // Père : attend la fin du fils
        int status = 0;
        pid_t w = waitpid(pid, &status, 0);
        if (w == -1) {
            perror("waitpid");
            return EXIT_FAILURE;
        }
        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            printf("[PERE ] Fils %d terminé normalement avec code=%d\n", pid, code);
        } else if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            printf("[PERE ] Fils %d tué par le signal %d\n", pid, sig);
        } else {
            printf("[PERE ] Fils %d terminé d'une autre manière\n", pid);
        }
    }
    return EXIT_SUCCESS;
}
