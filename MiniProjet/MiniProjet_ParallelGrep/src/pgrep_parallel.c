// src/pgrep_parallel.c
// Parallel Grep: recherche d'un motif dans les fichiers d'un répertoire (récursif par défaut).
// Couvre : mmap (mémoire), pthreads (multi-thread), open/read/write/close (FS), permissions (umask + 0640).

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct Task {
    char path[PATH_MAX];
    struct Task* next;
} Task;

typedef struct {
    Task* head;
    Task* tail;
    pthread_mutex_t mtx;
    pthread_cond_t cv;
    int closed; // plus de nouvelles tâches
} TaskQueue;

static void tq_init(TaskQueue* q) {
    q->head = q->tail = NULL;
    q->closed = 0;
    pthread_mutex_init(&q->mtx, NULL);
    pthread_cond_init(&q->cv, NULL);
}
static void tq_close(TaskQueue* q) {
    pthread_mutex_lock(&q->mtx);
    q->closed = 1;
    pthread_cond_broadcast(&q->cv);
    pthread_mutex_unlock(&q->mtx);
}
static void tq_push(TaskQueue* q, const char* path) {
    Task* t = malloc(sizeof(Task));
    if (!t) { perror("malloc"); exit(1); }
    strncpy(t->path, path, PATH_MAX-1);
    t->path[PATH_MAX-1] = '\0';
    t->next = NULL;
    pthread_mutex_lock(&q->mtx);
    if (q->tail) q->tail->next = t; else q->head = t;
    q->tail = t;
    pthread_cond_signal(&q->cv);
    pthread_mutex_unlock(&q->mtx);
}
static int tq_pop(TaskQueue* q, char* out) {
    pthread_mutex_lock(&q->mtx);
    while (!q->head && !q->closed) {
        pthread_cond_wait(&q->cv, &q->mtx);
    }
    if (!q->head && q->closed) { pthread_mutex_unlock(&q->mtx); return 0; }
    Task* t = q->head;
    q->head = t->next;
    if (!q->head) q->tail = NULL;
    pthread_mutex_unlock(&q->mtx);
    strncpy(out, t->path, PATH_MAX);
    free(t);
    return 1;
}

typedef struct {
    TaskQueue* q;
    const char* pattern;
    int out_fd;
    pthread_mutex_t* log_mtx;
    volatile unsigned long long *files_done;
    volatile unsigned long long *matches_total;
} WorkerArgs;

// Compte les occurrences de "pattern" dans [data, data+len)
static size_t count_occurrences(const char* data, size_t len, const char* pattern) {
    size_t m = strlen(pattern);
    if (m == 0 || len < m) return 0;
    size_t count = 0;
    // Recherche naïve (suffisant pédagogiquement). Optimisable (KMP/BM).
    for (size_t i = 0; i + m <= len; ++i) {
        if (memcmp(data + i, pattern, m) == 0) count++;
    }
    return count;
}

static void write_csv_line(int fd, pthread_mutex_t* mtx, const char* path, off_t size, size_t occ) {
    char line[PATH_MAX + 128];
    int n = snprintf(line, sizeof(line), "\"%s\",%lld,%zu\n", path, (long long)size, occ);
    pthread_mutex_lock(mtx);
    if (write(fd, line, n) < 0) perror("write(results)");
    pthread_mutex_unlock(mtx);
}

static void* worker(void* p) {
    WorkerArgs* a = (WorkerArgs*)p;
    char path[PATH_MAX];
    while (tq_pop(a->q, path)) {
        int fd = open(path, O_RDONLY);
        if (fd < 0) { /* fichier peut disparaître */ continue; }

        struct stat st;
        if (fstat(fd, &st) < 0) { close(fd); continue; }
        if (!S_ISREG(st.st_mode)) { close(fd); continue; }

        size_t occ = 0;
        if (st.st_size > 0) {
            void* data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (data == MAP_FAILED) {
                // fallback: buffer (rare)
                const size_t BUF = 1<<20;
                char *buf = malloc(BUF);
                if (!buf) { close(fd); continue; }
                ssize_t n;
                while ((n = read(fd, buf, BUF)) > 0) {
                    occ += count_occurrences(buf, (size_t)n, a->pattern);
                }
                free(buf);
            } else {
                occ += count_occurrences((const char*)data, st.st_size, a->pattern);
                munmap(data, st.st_size);
            }
        }

        write_csv_line(a->out_fd, a->log_mtx, path, st.st_size, occ);
        __sync_fetch_and_add(a->files_done, 1);
        __sync_fetch_and_add(a->matches_total, occ);

        close(fd);
    }
    return NULL;
}

static int is_dir(const char* path) {
    struct stat st;
    if (stat(path, &st) < 0) return 0;
    return S_ISDIR(st.st_mode);
}

static void walk_dir(TaskQueue* q, const char* root, int recursive) {
    DIR* d = opendir(root);
    if (!d) return;
    struct dirent* ent;
    char path[PATH_MAX];
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        snprintf(path, sizeof(path), "%s/%s", root, ent->d_name);
        struct stat st;
        if (lstat(path, &st) < 0) continue;
        if (S_ISLNK(st.st_mode)) continue; // ignorer les liens symboliques pour éviter les boucles
        if (S_ISDIR(st.st_mode)) {
            if (recursive) walk_dir(q, path, recursive);
        } else if (S_ISREG(st.st_mode)) {
            tq_push(q, path);
        }
    }
    closedir(d);
}

static void usage(const char* prog) {
    fprintf(stderr, "Usage: %s -p <pattern> [-d <dir>] [-t <threads>] [-o <output.csv>] [-x]\n", prog);
}

int main(int argc, char** argv) {
    const char* pattern = NULL;
    const char* root = ".";
    const char* outpath = "results.csv";
    int threads = 0;
    int recursive = 1;

    int opt;
    while ((opt = getopt(argc, argv, "p:d:t:o:x")) != -1) {
        switch (opt) {
            case 'p': pattern = optarg; break;
            case 'd': root = optarg; break;
            case 't': threads = atoi(optarg); break;
            case 'o': outpath = optarg; break;
            case 'x': recursive = 0; break;
            default: usage(argv[0]); return 1;
        }
    }
    if (!pattern) { usage(argv[0]); return 1; }
    if (threads <= 0) {
        long ncpu = sysconf(_SC_NPROCESSORS_ONLN);
        threads = (ncpu > 0) ? (int)ncpu : 4;
    }

    // Préparer fichier CSV : rw-r----- (0640)
    mode_t old = umask(027);
    int out_fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    umask(old);
    if (out_fd < 0) { perror("open(output)"); return 1; }

    // En-tête CSV
    const char *hdr = "path,size_bytes,occurrences\n";
    if (write(out_fd, hdr, strlen(hdr)) < 0) perror("write(header)");

    TaskQueue q; tq_init(&q);
    pthread_mutex_t log_mtx; pthread_mutex_init(&log_mtx, NULL);

    volatile unsigned long long files_done = 0;
    volatile unsigned long long matches_total = 0;

    // D démarrage : parcourir le FS et remplir la file
    if (is_dir(root)) {
        walk_dir(&q, root, recursive);
    } else {
        tq_push(&q, root);
    }
    tq_close(&q);

    // L travailleurs
    pthread_t *ths = malloc(sizeof(pthread_t) * threads);
    WorkerArgs a = { .q=&q, .pattern=pattern, .out_fd=out_fd, .log_mtx=&log_mtx,
                     .files_done=&files_done, .matches_total=&matches_total };

    for (int i=0;i<threads;i++) pthread_create(&ths[i], NULL, worker, &a);
    for (int i=0;i<threads;i++) pthread_join(ths[i], NULL);

    dprintf(out_fd, "# summary,files=%llu,matches=%llu\n",
            (unsigned long long)files_done, (unsigned long long)matches_total);

    close(out_fd);
    free(ths);
    pthread_mutex_destroy(&log_mtx);
    return 0;
}
