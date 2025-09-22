// ex2_controller.c
// TP2 – Exercice 2 (optionnel) : Contrôleur qui envoie STOP/CONT/KILL au fils.
// Utile si vous préférez automatiser au lieu d'utiliser la commande `kill`.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex2_controller.c -o ex2_controller
// Exécution   : ./ex2_controller
//
// Comportement :
//   - fork d'un fils (ex2_loop style)
//   - parent : STOP -> attendre 2s -> CONT -> attendre 2s -> KILL -> waitpid

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        // Fils : boucle simple
        fprintf(stderr, "[FILS] PID=%d prêt (boucle infinie)\n", getpid());
        volatile unsigned long long x = 0;
        while (1) { x++; if ((x & ((1ULL<<26)-1))==0) usleep(50000); } // petit throttle
        _exit(0);
    } else {
        fprintf(stderr, "[PERE] PID=%d, fils=%d\n", getpid(), pid);
        sleep(1);
        fprintf(stderr, "[PERE] STOP du fils...\n");
        if (kill(pid, SIGSTOP) < 0) perror("kill(SIGSTOP)");
        sleep(2);
        fprintf(stderr, "[PERE] CONT du fils...\n");
        if (kill(pid, SIGCONT) < 0) perror("kill(SIGCONT)");
        sleep(2);
        fprintf(stderr, "[PERE] KILL du fils...\n");
        if (kill(pid, SIGKILL) < 0) perror("kill(SIGKILL)");
        int status=0;
        if (waitpid(pid, &status, 0) < 0) perror("waitpid");
        else {
            if (WIFSIGNALED(status))
                fprintf(stderr, "[PERE] Fils terminé par signal %d\n", WTERMSIG(status));
            else if (WIFEXITED(status))
                fprintf(stderr, "[PERE] Fils exit code=%d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}
