// minishell.c
// TP2 – Mini-shell POSIX (fork/exec/wait, pipes, redirections, cd/exit)
// Usage: ./minishell
// Dépendances: POSIX (Linux/macOS/WSL). Compiler avec gcc/clang.
//
// Fonctionnalités :
//  - Boucle REPL: lit une ligne, exécute la commande
//  - Built-ins: cd [dir], exit
//  - Exécution de commandes avec arguments (execvp)
//  - Pipe simple: commande1 | commande2
//  - Redirections: > fichier (stdout), < fichier (stdin), >> append
//  - Gestion de quelques erreurs courantes
//
// Limites :
//  - Parsing simple: séparateur espace, guillemets non gérés (ex: "a b") -> à améliorer en extension
//  - Un seul pipe '|' supporté (extension possible)
//  - Pas de job control (&, fg/bg) pour rester simple
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 minishell.c -o minishell
// Exécution   : ./minishell
//
// Exemples :
//  - ls -l | grep '\.c'
//  - cat < minishell.c | wc -l
//  - echo hello > out.txt
//  - cd ..
//  - exit

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_ARGS 128

typedef struct Cmd {
    char *argv[MAX_ARGS];
    char *redir_in;      // fichier pour <
    char *redir_out;     // fichier pour > ou >>
    int   append_out;    // 0 => > , 1 => >>
} Cmd;

// Trim leading/trailing spaces (in place)
static char* trim(char *s) {
    if (!s) return s;
    while (*s && (*s==' '||*s=='\t'||*s=='\n')) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end==' '||*end=='\t'||*end=='\n')) *end-- = '\0';
    return s;
}

// Split a command segment into argv and redirections. Returns 0 on success.
static int parse_command(char *segment, Cmd *cmd) {
    memset(cmd, 0, sizeof(*cmd));
    char *tok = strtok(segment, " \t");
    int argc = 0;
    while (tok && argc < MAX_ARGS-1) {
        if (strcmp(tok, "<") == 0) {
            tok = strtok(NULL, " \t");
            if (!tok) { fprintf(stderr, "Erreur: manquant après '<'\n"); return -1; }
            cmd->redir_in = tok;
        } else if (strcmp(tok, ">") == 0) {
            tok = strtok(NULL, " \t");
            if (!tok) { fprintf(stderr, "Erreur: manquant après '>'\n"); return -1; }
            cmd->redir_out = tok;
            cmd->append_out = 0;
        } else if (strcmp(tok, ">>") == 0) {
            tok = strtok(NULL, " \t");
            if (!tok) { fprintf(stderr, "Erreur: manquant après '>>'\n"); return -1; }
            cmd->redir_out = tok;
            cmd->append_out = 1;
        } else {
            cmd->argv[argc++] = tok;
        }
        tok = strtok(NULL, " \t");
    }
    cmd->argv[argc] = NULL;
    return 0;
}

static int is_builtin(Cmd *cmd) {
    if (!cmd->argv[0]) return 0;
    return (strcmp(cmd->argv[0], "cd") == 0) || (strcmp(cmd->argv[0], "exit") == 0);
}

static int run_builtin(Cmd *cmd) {
    if (strcmp(cmd->argv[0], "cd") == 0) {
        const char *dir = cmd->argv[1] ? cmd->argv[1] : getenv("HOME");
        if (!dir) dir = ".";
        if (chdir(dir) != 0) {
            perror("cd");
            return -1;
        }
        return 0;
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        int code = 0;
        if (cmd->argv[1]) code = atoi(cmd->argv[1]);
        exit(code);
    }
    return -1;
}

// Exécute une commande simple (sans pipe), avec redirections
static int exec_simple(Cmd *cmd) {
    if (!cmd->argv[0]) return 0;

    if (is_builtin(cmd)) {
        return run_builtin(cmd);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        // Fils: appliquer redirections
        if (cmd->redir_in) {
            int fd = open(cmd->redir_in, O_RDONLY);
            if (fd < 0) { perror("open(<)"); _exit(126); }
            if (dup2(fd, STDIN_FILENO) < 0) { perror("dup2(<)"); _exit(126); }
            close(fd);
        }
        if (cmd->redir_out) {
            int flags = O_WRONLY | O_CREAT | (cmd->append_out ? O_APPEND : O_TRUNC);
            int fd = open(cmd->redir_out, flags, 0644);
            if (fd < 0) { perror("open(>)"); _exit(126); }
            if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2(>)"); _exit(126); }
            close(fd);
        }
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        _exit(127);
    } else {
        int status = 0;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
            return -1;
        }
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Signal: %d\n", WTERMSIG(status));
            return 128 + WTERMSIG(status);
        }
    }
    return 0;
}

// Exécute une commande avec un pipe simple: left | right
static int exec_pipe(Cmd *left, Cmd *right) {
    int fds[2];
    if (pipe(fds) < 0) { perror("pipe"); return -1; }

    // Processus gauche
    pid_t pid1 = fork();
    if (pid1 < 0) { perror("fork"); return -1; }
    if (pid1 == 0) {
        // redirections du left + stdout -> pipe
        if (left->redir_in) {
            int fd = open(left->redir_in, O_RDONLY);
            if (fd < 0) { perror("open(<)"); _exit(126); }
            if (dup2(fd, STDIN_FILENO) < 0) { perror("dup2(<)"); _exit(126); }
            close(fd);
        }
        if (left->redir_out) {
            // autoriser "left ... > f | right" (peu courant mais possible)
            int flags = O_WRONLY | O_CREAT | (left->append_out ? O_APPEND : O_TRUNC);
            int fd = open(left->redir_out, flags, 0644);
            if (fd < 0) { perror("open(> left)"); _exit(126); }
            if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2(> left)"); _exit(126); }
            // ne pas oublier de fermer fd après dup2
            close(fd);
        } else {
            if (dup2(fds[1], STDOUT_FILENO) < 0) { perror("dup2(pipe write)"); _exit(126); }
        }
        close(fds[0]); close(fds[1]);
        if (is_builtin(left)) {
            // exécuter builtin dans le sous-processus pour uniformité
            run_builtin(left);
            _exit(0);
        } else {
            execvp(left->argv[0], left->argv);
            perror("execvp(left)");
            _exit(127);
        }
    }

    // Processus droit
    pid_t pid2 = fork();
    if (pid2 < 0) { perror("fork"); return -1; }
    if (pid2 == 0) {
        if (right->redir_out) {
            int flags = O_WRONLY | O_CREAT | (right->append_out ? O_APPEND : O_TRUNC);
            int fd = open(right->redir_out, flags, 0644);
            if (fd < 0) { perror("open(> right)"); _exit(126); }
            if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2(> right)"); _exit(126); }
            close(fd);
        }
        if (right->redir_in) {
            int fd = open(right->redir_in, O_RDONLY);
            if (fd < 0) { perror("open(< right)"); _exit(126); }
            if (dup2(fd, STDIN_FILENO) < 0) { perror("dup2(< right)"); _exit(126); }
            close(fd);
        } else {
            if (dup2(fds[0], STDIN_FILENO) < 0) { perror("dup2(pipe read)"); _exit(126); }
        }
        close(fds[0]); close(fds[1]);
        if (is_builtin(right)) {
            run_builtin(right);
            _exit(0);
        } else {
            execvp(right->argv[0], right->argv);
            perror("execvp(right)");
            _exit(127);
        }
    }

    close(fds[0]); close(fds[1]);
    int status1=0, status2=0;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    // renvoyer le code de la commande de droite (comme les shells)
    if (WIFEXITED(status2)) return WEXITSTATUS(status2);
    if (WIFSIGNALED(status2)) return 128 + WTERMSIG(status2);
    return 0;
}

int main(void) {
    char *line = NULL;
    size_t cap = 0;

    while (1) {
        // Prompt simple
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd))) {
            printf("mini:%s$ ", cwd);
        } else {
            printf("mini$ ");
        }
        fflush(stdout);

        ssize_t n = getline(&line, &cap, stdin);
        if (n < 0) { // EOF (Ctrl-D)
            printf("\n");
            break;
        }
        // retirer le \n final
        if (n > 0 && line[n-1] == '\n') line[n-1] = '\0';

        char *input = trim(line);
        if (*input == '\0') continue; // ligne vide

        // Séparer pipe éventuel
        char *bar = strchr(input, '|');
        if (!bar) {
            // Pas de pipe
            Cmd c;
            if (parse_command(input, &c) != 0) continue;
            exec_simple(&c);
        } else {
            // Pipe simple left | right
            *bar = '\0';
            char *left_s  = trim(input);
            char *right_s = trim(bar+1);
            if (*left_s=='\0' || *right_s=='\0') {
                fprintf(stderr, "Syntaxe pipe invalide\n");
                continue;
            }
            Cmd left, right;
            if (parse_command(left_s, &left) != 0) continue;
            if (parse_command(right_s, &right) != 0) continue;
            exec_pipe(&left, &right);
        }
    }
    free(line);
    return 0;
}
