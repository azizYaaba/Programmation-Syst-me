// minishell_plus.c
// TP7 – Mini-projet : Mini-shell avancé
//
// Objectifs :
//  - Créer un processus fils pour exécuter une commande (execvp).
//  - Supporter la redirection et les pipes (plusieurs '|' possibles).
//  - Gérer la terminaison des processus (attente des pipelines, éviter les zombies).
//  - Built-ins de base : cd, exit.
//  - Gestion de SIGINT : Ctrl-C interrompt le pipeline au premier plan, pas le shell.
//
// Limites (volontaires pour rester pédagogique) :
//  - Parsing simplifié des guillemets : prise en charge minimale des guillemets doubles "..." (sans échappements).
//  - Pas de variables d'environnement ni de substitution, pas de job control (&).
//
// Compilation : gcc -Wall -Wextra -O2 -std=c11 minishell_plus.c -o minishell_plus
// Exécution   : ./minishell_plus
//
// Exemples :
//   ls -l | grep '\\.c' | wc -l
//   cat < minishell_plus.c | grep exec > out.txt
//   echo hello > a.txt ; cat a.txt
//
// Astuces de test :
//   - strace -f ./minishell_plus
//   - lsof -p $(pidof minishell_plus) (Linux) pour vérifier les descripteurs ouverts
//
// © TP pédagogique POSIX

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define MAX_CMDS   64
#define MAX_ARGS   128

typedef struct Cmd {
    char *argv[MAX_ARGS];
    char *redir_in;      // <
    char *redir_out;     // >  ou >>
    int   append_out;    // 1 si >>
    char *redir_err;     // 2> ou 2>>
    int   append_err;    // 1 si 2>>
    int   merge_err_to_out; // 2>&1
} Cmd;

static volatile sig_atomic_t child_running = 0;

static void on_sigchld(int sig) {
    (void)sig;
    // Récolter tous les enfants terminés (éviter zombies)
    int saved = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
    errno = saved;
}

static void on_sigint(int sig) {
    (void)sig;
    if (!child_running) {
        // On est au prompt : juste aller à la ligne
        write(STDOUT_FILENO, "\n", 1);
    }
    // Sinon, le SIGINT sera reçu par les enfants (par défaut) -> ils s'interrompent
}

static char* trim(char *s) {
    if (!s) return s;
    while (*s && (*s==' '||*s=='\t'||*s=='\n')) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end==' '||*end=='\t'||*end=='\n')) *end-- = '\0';
    return s;
}

// Tokenizer simple avec support minimal des guillemets doubles (sans échappement)
static char* next_token(char **ps) {
    char *s = *ps;
    while (*s==' '||*s=='\t') s++;
    if (*s=='\0') { *ps = s; return NULL; }

    if (*s=='\"') {
        s++;
        char *start = s;
        while (*s && *s!='\"') s++;
        size_t len = s - start;
        char *tok = strndup(start, len);
        if (*s=='\"') s++;
        *ps = s;
        return tok;
    }

    char *start = s;
    while (*s && *s!=' ' && *s!='\t' && *s!='|' ) {
        // Redirections sont traitées en amont par parse_command
        if (*s=='<') break;
        if (*s=='>') break;
        if (*s=='2') break;
        s++;
    }
    size_t len = s - start;
    if (len == 0) { // opérateur spécial devant nous
        // Laisser parse_command regarder
        *ps = s;
        return NULL;
    }
    char *tok = strndup(start, len);
    *ps = s;
    return tok;
}

// Analyse une commande simple (sans pipe), gère redirections
static int parse_command(char *segment, Cmd *cmd) {
    memset(cmd, 0, sizeof(*cmd));
    char *p = segment;
    int argc = 0;

    while (1) {
        while (*p==' '||*p=='\t') p++;
        if (*p=='\0') break;

        if (*p=='<') {
            p++;
            char *tok = next_token(&p);
            if (!tok) { fprintf(stderr, "Erreur: nom manquant après '<'\n"); return -1; }
            cmd->redir_in = tok;
            continue;
        }
        if (*p=='>') {
            p++;
            if (*p=='>') { cmd->append_out = 1; p++; }
            char *tok = next_token(&p);
            if (!tok) { fprintf(stderr, "Erreur: nom manquant après '>'\n"); return -1; }
            cmd->redir_out = tok;
            continue;
        }
        if (*p=='2') {
            p++;
            if (*p=='>') {
                p++;
                if (*p=='>') { cmd->append_err = 1; p++; }
                if (*p=='&') { // 2>&1
                    p++;
                    if (*p=='1') { p++; cmd->merge_err_to_out = 1; }
                    else { fprintf(stderr, "Syntaxe attendue: 2>&1\n"); return -1; }
                } else {
                    char *tok = next_token(&p);
                    if (!tok) { fprintf(stderr, "Erreur: nom manquant après '2>'\n"); return -1; }
                    cmd->redir_err = tok;
                }
                continue;
            } else {
                fprintf(stderr, "Syntaxe invalide près de '2'\n");
                return -1;
            }
        }

        char *tok = next_token(&p);
        if (tok) {
            if (argc < MAX_ARGS-1) cmd->argv[argc++] = tok;
            else { fprintf(stderr, "Trop d'arguments\n"); return -1; }
        } else {
            // Avancer d'un caractère (opérateur) pour éviter boucle infinie
            p++;
        }
    }
    cmd->argv[argc] = NULL;
    return 0;
}

// Split sur '|' en commandes élémentaires
static int split_pipeline(char *line, char *parts[], int max_parts) {
    int n = 0;
    char *s = line;
    while (*s) {
        char *start = s;
        int quotes = 0;
        while (*s) {
            if (*s=='\"') quotes = !quotes;
            if (!quotes && *s=='|') break;
            s++;
        }
        size_t len = s - start;
        char *seg = strndup(start, len);
        parts[n++] = seg;
        if (n == max_parts) break;
        if (*s=='|') s++;
        while (*s==' ') s++;
        if (*s=='\0') break;
    }
    return n;
}

static int is_builtin(Cmd *cmd) {
    if (!cmd->argv[0]) return 0;
    return (strcmp(cmd->argv[0], "cd")==0) || (strcmp(cmd->argv[0], "exit")==0);
}

static int run_builtin(Cmd *cmd) {
    if (strcmp(cmd->argv[0], "cd")==0) {
        const char *dir = cmd->argv[1] ? cmd->argv[1] : getenv("HOME");
        if (!dir) dir = ".";
        if (chdir(dir) != 0) { perror("cd"); return 1; }
        return 0;
    } else if (strcmp(cmd->argv[0], "exit")==0) {
        int code = 0;
        if (cmd->argv[1]) code = atoi(cmd->argv[1]);
        exit(code);
    }
    return 1;
}

static void apply_redirs(Cmd *c) {
    if (c->redir_in) {
        int fd = open(c->redir_in, O_RDONLY);
        if (fd < 0) { perror("open(<)"); _exit(126); }
        if (dup2(fd, STDIN_FILENO) < 0) { perror("dup2(<)"); _exit(126); }
        close(fd);
    }
    if (c->redir_out) {
        int flags = O_WRONLY | O_CREAT | (c->append_out ? O_APPEND : O_TRUNC);
        int fd = open(c->redir_out, flags, 0644);
        if (fd < 0) { perror("open(>)"); _exit(126); }
        if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2(>)"); _exit(126); }
        close(fd);
    }
    if (c->merge_err_to_out) {
        if (dup2(STDOUT_FILENO, STDERR_FILENO) < 0) { perror("dup2(2>&1)"); _exit(126); }
    } else if (c->redir_err) {
        int flags = O_WRONLY | O_CREAT | (c->append_err ? O_APPEND : O_TRUNC);
        int fd = open(c->redir_err, flags, 0644);
        if (fd < 0) { perror("open(2>)"); _exit(126); }
        if (dup2(fd, STDERR_FILENO) < 0) { perror("dup2(2>)"); _exit(126); }
        close(fd);
    }
}

static int exec_pipeline(Cmd cmds[], int ncmds) {
    int pipes[MAX_CMDS-1][2];
    for (int i=0; i<ncmds-1; ++i) {
        if (pipe(pipes[i]) < 0) { perror("pipe"); return 1; }
    }

    pid_t pids[MAX_CMDS];
    for (int i=0; i<ncmds; ++i) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return 1; }
        if (pid == 0) {
            // Enfant : restaurer SIGINT par défaut
            signal(SIGINT, SIG_DFL);
            // Connexions pipe
            if (i > 0) { // lire depuis pipe précédent
                if (dup2(pipes[i-1][0], STDIN_FILENO) < 0) { perror("dup2(in)"); _exit(126); }
            }
            if (i < ncmds-1) { // écrire vers pipe suivant
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0) { perror("dup2(out)"); _exit(126); }
            }
            // Fermer tous les descripteurs de pipe
            for (int k=0; k<ncmds-1; ++k) { close(pipes[k][0]); close(pipes[k][1]); }

            // Appliquer redirections (peuvent écraser STDIN/STDOUT/ERR configurés ci-dessus)
            apply_redirs(&cmds[i]);

            if (is_builtin(&cmds[i])) {
                // Built-ins dans un enfant (ne pas affecter le shell)
                _exit(run_builtin(&cmds[i]));
            }
            execvp(cmds[i].argv[0], cmds[i].argv);
            perror("execvp");
            _exit(127);
        } else {
            pids[i] = pid;
        }
    }
    // Parent : fermer les pipes
    for (int k=0; k<ncmds-1; ++k) { close(pipes[k][0]); close(pipes[k][1]); }

    // Attendre tous les enfants ; code de retour = code du dernier (comme shells)
    int status = 0, last_status = 0;
    child_running = 1;
    for (int i=0; i<ncmds; ++i) {
        if (waitpid(pids[i], &status, 0) < 0) perror("waitpid");
        if (i == ncmds-1) last_status = status;
    }
    child_running = 0;

    if (WIFEXITED(last_status)) return WEXITSTATUS(last_status);
    if (WIFSIGNALED(last_status)) return 128 + WTERMSIG(last_status);
    return 0;
}

int main(void) {
    // Installer gestionnaires de signaux
    struct sigaction sa = {0};
    sa.sa_handler = on_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    signal(SIGINT, on_sigint);

    char *line = NULL;
    size_t cap = 0;

    while (1) {
        // Prompt
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd))) printf("msh:%s$ ", cwd);
        else                          printf("msh$ ");
        fflush(stdout);

        ssize_t n = getline(&line, &cap, stdin);
        if (n < 0) { printf("\n"); break; }
        if (n > 0 && line[n-1]=='\n') line[n-1]='\0';

        char *input = trim(line);
        if (*input=='\0') continue;

        // Supporter ';' pour enchaîner plusieurs commandes simples (option)
        char *cursor = input;
        while (cursor && *cursor) {
            char *sep = strchr(cursor, ';');
            char *one = NULL;
            if (sep) {
                *sep = '\0';
                one = cursor;
                cursor = sep + 1;
            } else {
                one = cursor;
                cursor = NULL;
            }
            one = trim(one);
            if (*one=='\0') continue;

            // Découper le pipeline
            char *parts[MAX_CMDS] = {0};
            int ncmds = split_pipeline(one, parts, MAX_CMDS);
            if (ncmds <= 0) continue;

            // Parser chaque segment
            Cmd cmds[MAX_CMDS] = {{0}};
            int ok = 1;
            for (int i=0; i<ncmds; ++i) {
                char *seg = trim(parts[i]);
                if (parse_command(seg, &cmds[i]) != 0) { ok = 0; }
                if (!cmds[i].argv[0]) { fprintf(stderr, "Commande vide\n"); ok = 0; }
            }
            if (!ok) {
                for (int i=0; i<ncmds; ++i) free(parts[i]);
                continue;
            }

            // Optimisation: si une seule commande ET builtin -> exécuter dans le shell
            if (ncmds == 1 && is_builtin(&cmds[0])) {
                run_builtin(&cmds[0]);
                for (int i=0; i<ncmds; ++i) free(parts[i]);
                continue;
            }

            // Exécuter le pipeline
            exec_pipeline(cmds, ncmds);
            for (int i=0; i<ncmds; ++i) free(parts[i]);
        }
    }

    free(line);
    return 0;
}
