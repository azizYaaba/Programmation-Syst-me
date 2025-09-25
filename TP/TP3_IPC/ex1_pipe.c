// ex1_pipe.c
// TP4 – IPC — Exercice 1: Pipe père -> fils
// Objectif: le père envoie un message via un pipe, le fils le lit.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex1_pipe.c -o ex1_pipe
// Exécution   : ./ex1_pipe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    int fd[2];
    if (pipe(fd) == -1) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // --- FILS : lecture ---
        //close(fd[1]); // ne garde pas l'écriture
        char buf[256] = {0};
        ssize_t n = read(fd[0], buf, sizeof(buf)-1);
        if (n < 0) { perror("read"); return 1; }
        printf("[FILS] Recu (%zd octets): \"%s\"\n", n, buf);
        close(fd[0]);
    } else {
        // --- PERE : écriture ---
        close(fd[0]); // ne garde pas la lecture
        const char *msg = "Bonjour du père via pipe !";
        ssize_t n = write(fd[1], msg, strlen(msg));
        if (n < 0) { perror("write"); return 1; }
        printf("[PERE] Envoye (%zd octets)\n", n);
        close(fd[1]);
    }
    return 0;
}
