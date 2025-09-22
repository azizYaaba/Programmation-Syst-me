# TP1 – Découverte des processus (fork, exec, wait)

## Objectifs
- Comprendre la création de processus avec `fork()`.
- Remplacer l’image d’un processus avec `exec()`.
- Synchroniser le père avec le fils via `wait()` / `waitpid()`.

## Prérequis
- Environnement POSIX (Linux, macOS, WSL).
- `gcc` installé (ou `clang`).
- Accès au shell.

## Contenu
- `ex1_pid.c` : création de processus, affichage des PID.
- `ex2_exec_ls.c` : `exec()` pour exécuter `ls -l` dans le fils.
- `ex3_wait.c` : le père attend la fin du fils, récupère le code de retour.
- `ex_all.c` : version combinant les trois exercices.
- `Makefile` : compilation simplifiée.

## Compilation
```bash
make        # compile tous les exercices
# ou
gcc -Wall -Wextra -O2 -std=c11 ex1_pid.c -o ex1_pid
gcc -Wall -Wextra -O2 -std=c11 ex2_exec_ls.c -o ex2_exec_ls
gcc -Wall -Wextra -O2 -std=c11 ex3_wait.c -o ex3_wait
gcc -Wall -Wextra -O2 -std=c11 ex_all.c -o ex_all
```

## Exécution
```bash
./ex1_pid
./ex2_exec_ls
./ex3_wait
./ex_all
```

### Sorties attendues (exemples)
- `ex1_pid` :
```
[PERE ] PID=12345, fils=12346
[FILS ] PID=12346, PPID=12345
```

- `ex2_exec_ls` :
```
[PERE ] PID=12345, fils=12346 (le fils devrait lister le répertoire courant)
# puis la sortie de "ls -l"
```

- `ex3_wait` :
```
# sortie de "ls -l" (fils)
[PERE ] Fils 12346 terminé normalement avec code=0
```

- `ex_all` :
```
== Ex1 : fork & PIDs ==
[PERE ] PID=12345, fils=12346
[FILS ] PID=12346, PPID=12345
== Ex2 : exec ls -l (dans le fils) ==
# sortie de "ls -l"
== Ex3 : wait du père ==
[PERE ] Fils 12346 terminé (code=0)
```

## Étapes pédagogiques
1. **Ex1** : observer que `fork()` s’exécute 2 fois (chemin père/fils).
2. **Ex2** : montrer que `exec()` remplace le code du fils (lignes après `execlp` non exécutées si succès).
3. **Ex3** : synchroniser via `waitpid()` et interpréter `status` (`WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`).

## Vérification / Debug
- `echo $$` pour connaître le PID du shell.
- `ps -o pid,ppid,stat,cmd -p <pid>` pour inspecter un processus.
- `strace ./ex3_wait` (Linux) pour tracer `fork/execve/wait4`.
- Débrancher `ls` (ex. `execlp("ls_inexistant", ...)`) pour voir le cas d’échec (`_exit(127)`).

## Extensions (facultatif)
- Remplacer `ls -l` par une commande et des arguments saisis par l’utilisateur.
- Faire plusieurs `fork()` pour créer un arbre de processus.
- Implémenter un mini-shell qui lit une ligne et exécute la commande correspondante.

## Notes
- Utiliser `_exit()` côté fils après un `exec` échoué pour éviter d’exécuter le code du père par inadvertance.
- Préférer `execlp`/`execvp` pour bénéficier de `PATH`.
