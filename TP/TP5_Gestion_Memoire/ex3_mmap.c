// ex3_mmap.c
// TP6 – Gestion mémoire — Exercice 3
// Objectif : mapper un fichier en mémoire avec mmap() et afficher son contenu.
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 ex3_mmap.c -o ex3_mmap
// Exécution   : ./ex3_mmap <fichier> [max_octets_a_afficher]
//
// Exemple :
//   echo "Hello mmap!" > data.txt
//   ./ex3_mmap data.txt
//
// Notes :
//  - mappe le fichier en lecture seule (PROT_READ | MAP_PRIVATE)
//  - affiche au plus N octets (défaut 256) pour éviter d'inonder la sortie

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

static void hexdump(const unsigned char *p, size_t n) {
    for (size_t i = 0; i < n; i += 16) {
        printf("%08zx  ", i);
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < n) printf("%02x ", p[i + j]);
            else printf("   ");
        }
        printf(" |");
        for (size_t j = 0; j < 16 && i + j < n; ++j) {
            unsigned char c = p[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        printf("|\n");
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier> [max_octets]\n", argv[0]);
        return 1;
    }
    const char *path = argv[1];
    long max_out = (argc > 2) ? strtol(argv[2], NULL, 10) : 256;
    if (max_out <= 0 || max_out > 10*1024*1024) max_out = 256;

    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    struct stat st;
    if (fstat(fd, &st) != 0) { perror("fstat"); close(fd); return 1; }
    if (st.st_size == 0) { fprintf(stderr, "Fichier vide.\n"); close(fd); return 1; }

    size_t len = (size_t)st.st_size;
    unsigned char *map = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    size_t to_show = len < (size_t)max_out ? len : (size_t)max_out;
    printf("Fichier: %s (%zu octets). Affiche %zu octets.\n", path, len, to_show);
    hexdump(map, to_show);

    if (munmap(map, len) != 0) perror("munmap");
    close(fd);
    return 0;
}
