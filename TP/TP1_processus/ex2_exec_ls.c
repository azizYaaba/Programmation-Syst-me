// ex2_exec_ls.c
// TP1 – Découverte des processus — Exercice 2
// Objectif : le fils exécute "ls -l" via exec(), le père affiche son PID.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex2_exec_ls.c -o ex2_exec_ls
// Exécution   : ./ex2_exec_ls

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
        // Code du fils : remplace son image par /bin/ls -l
        // execlp recherche "ls" dans PATH
        execlp("ls", "ls", "-l", (char*)NULL);
        // Si on arrive ici, exec a échoué
        perror("execlp(ls)");
        _exit(127); // code d'erreur conventionnel si exec échoue
    } else {
        // Code du père : ne s'arrête pas ici (voir exercice 3 pour attendre)
        printf("[PERE ] PID=%d, fils=%d (le fils devrait lister le répertoire courant)\n", getpid(), pid);
    }
    return EXIT_SUCCESS;
}
