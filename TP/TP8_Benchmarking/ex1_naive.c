// ex1_naive.c — lecture caractère par caractère
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) { perror("open"); exit(1); }

    char c;
    while (read(fd, &c, 1) == 1) {
        volatile char x = c; // empêche optimisation
    }

    close(fd);
    return 0;
}
