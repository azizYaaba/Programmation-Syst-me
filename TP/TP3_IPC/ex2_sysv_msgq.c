// ex2_sysv_msgq.c
// TP4 – IPC — Exercice 2: File de messages System V
// Objectif: père -> fils via file de messages.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex2_sysv_msgq.c -o ex2_sysv_msgq
// Exécution   : ./ex2_sysv_msgq
//
// Outils: ipcs -q  |  ipcrm -q <id>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct msg {
    long mtype;          // type > 0
    char text[128];
};

int main(void) {
    // Créer une file de messages (clé IPC_PRIVATE pour éviter collision)
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid == -1) { perror("msgget"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // --- FILS : recevoir ---
        struct msg m;
        if (msgrcv(qid, &m, sizeof(m.text), 1, 0) == -1) {
            perror("msgrcv");
            _exit(1);
        }
        printf("[FILS] Message recu (type=%ld): %s\n", m.mtype, m.text);
        // nettoyage par le père (après wait)
        _exit(0);
    } else {
        // --- PERE : envoyer ---
        struct msg m = { .mtype = 1 };
        snprintf(m.text, sizeof(m.text), "Salut du pere via file de messages !");
        if (msgsnd(qid, &m, sizeof(m.text), 0) == -1) {
            perror("msgsnd");
            // essayer de supprimer la file si erreur
            msgctl(qid, IPC_RMID, NULL);
            return 1;
        }
        int status=0;
        waitpid(pid, &status, 0);
        // supprimer la file
        if (msgctl(qid, IPC_RMID, NULL) == -1) {
            perror("msgctl(IPC_RMID)");
            return 1;
        }
        printf("[PERE] File %d supprimee.\n", qid);
    }
    return 0;
}
