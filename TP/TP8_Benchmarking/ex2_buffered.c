// ex2_buffered.c — lecture par blocs
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) { perror("open"); exit(1); }

    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(fd, buf, BUF_SIZE)) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            volatile char x = buf[i]; // empêche optimisation
        }
    }

    close(fd);
    return 0;
}
