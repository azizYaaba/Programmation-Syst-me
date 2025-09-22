// ex2_append.c — Ecriture en mode ajout
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int fd = open("demo_append.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd < 0) { perror("open"); exit(1); }

    const char *msg = "Nouvelle ligne ajoutée.\n";
    if (write(fd, msg, strlen(msg)) < 0) { perror("write"); exit(1); }
    close(fd);
    return 0;
}
