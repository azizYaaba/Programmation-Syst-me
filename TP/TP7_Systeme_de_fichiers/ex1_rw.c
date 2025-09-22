// ex1_rw.c — Ouverture, écriture, lecture, fermeture d’un fichier
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int fd = open("demo.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); exit(1); }

    const char *msg = "Hello, system programming!\n";
    if (write(fd, msg, strlen(msg)) < 0) { perror("write"); exit(1); }
    close(fd);

    // Relecture
    fd = open("demo.txt", O_RDONLY);
    if (fd < 0) { perror("open"); exit(1); }

    char buf[128];
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n < 0) { perror("read"); exit(1); }
    buf[n] = '\0';
    printf("Lu depuis le fichier: %s", buf);
    close(fd);
    return 0;
}
