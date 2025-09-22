// ex_all.c
// TP1 – Découverte des processus — Version tout-en-un
// Ex1 : fork & PIDs | Ex2 : exec ls -l | Ex3 : wait du père
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex_all.c -o ex_all
// Exécution   : ./ex_all

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    printf("== Ex1 : fork & PIDs ==\n");
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        printf("[FILS ] PID=%d, PPID=%d\n", getpid(), getppid());
        // Ex2 : dans le fils, exécuter ls -l
        printf("== Ex2 : exec ls -l (dans le fils) ==\n");
        execlp("ls", "ls", "-l", (char*)NULL);
        perror("execlp(ls)");
        _exit(127);
    } else {
        printf("[PERE ] PID=%d, fils=%d\n", getpid(), pid);
        // Ex3 : le père attend la fin du fils
        printf("== Ex3 : wait du père ==\n");
        int status = 0;
        pid_t w = waitpid(pid, &status, 0);
        if (w == -1) {
            perror("waitpid");
            return EXIT_FAILURE;
        }
        if (WIFEXITED(status)) {
            printf("[PERE ] Fils %d terminé (code=%d)\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[PERE ] Fils %d tué par signal %d\n", pid, WTERMSIG(status));
        } else {
            printf("[PERE ] Fils %d terminé autrement\n", pid);
        }
    }
    return EXIT_SUCCESS;
}
