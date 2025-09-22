// ex3_mmap.c — lecture avec mmap
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
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

    struct stat st;
    if (fstat(fd, &st) < 0) { perror("fstat"); exit(1); }

    char *data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) { perror("mmap"); exit(1); }

    for (off_t i = 0; i < st.st_size; i++) {
        volatile char x = data[i]; // empêche optimisation
    }

    munmap(data, st.st_size);
    close(fd);
    return 0;
}
