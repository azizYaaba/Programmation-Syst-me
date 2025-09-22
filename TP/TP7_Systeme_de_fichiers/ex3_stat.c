// ex3_stat.c — Métadonnées d’un fichier
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    struct stat st;
    if (stat(argv[1], &st) < 0) {
        perror("stat");
        return 1;
    }

    printf("Taille: %ld octets\n", st.st_size);
    printf("Inode: %ld\n", st.st_ino);
    printf("Mode: %o\n", st.st_mode & 0777);
    printf("UID: %d, GID: %d\n", st.st_uid, st.st_gid);

    if (S_ISREG(st.st_mode)) printf("Type: fichier régulier\n");
    else if (S_ISDIR(st.st_mode)) printf("Type: répertoire\n");
    else if (S_ISLNK(st.st_mode)) printf("Type: lien symbolique\n");
    else printf("Autre type\n");

    return 0;
}
