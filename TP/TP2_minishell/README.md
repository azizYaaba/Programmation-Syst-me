# TP2 — Mini-Shell (fork/exec/wait, pipes, redirections)

## Objectifs
- Écrire un shell minimal :
  - Lire une commande (REPL)
  - Exécuter des binaires via `fork()` + `execvp()`
  - Attendre la fin des commandes (`waitpid()`)
  - Gérer `cd` et `exit`
  - Supporter un **pipe simple** `|` et les **redirections** `<`, `>`, `>>`

## Prérequis
- Environnement POSIX (Linux/macOS/WSL), `gcc` ou `clang`.

## Compilation
```bash
make
# ou
gcc -Wall -Wextra -O2 -std=c11 minishell.c -o minishell
```

## Exécution
```bash
./minishell
```

## Exemples à tester
```bash
ls -l
echo hello > out.txt
cat < minishell.c | wc -l
grep main < minishell.c > result.txt
ls -l | grep '\.c'
cd ..
exit 0
```

## Notes d’implémentation
- Parsing simple (séparation par espaces, **sans** gestion des guillemets) pour rester focalisé sur les syscalls/processus.
- Redirections :
  - `< fichier` : remplace `stdin` par le fichier (O_RDONLY)
  - `> fichier` : remplace `stdout` (O_WRONLY|O_CREAT|O_TRUNC, 0644)
  - `>> fichier` : append (O_WRONLY|O_CREAT|O_APPEND, 0644)
- Pipe simple : `commande1 | commande2`
  - `stdout` de `commande1` -> `stdin` de `commande2` via `pipe()` + `dup2()`
- Built-ins : `cd`, `exit` (exécutés dans le processus courant si commande simple ; dans un pipe, exécutés dans le sous-processus)

## Débogage / Vérifications
- `strace ./minishell` (Linux) : observer `fork`, `execve`, `wait4`, `pipe`, `dup2`, `open`…
- Tester les erreurs : chemin inexistant, permissions, fichiers manquants.
- Observer les descripteurs avec `ls -l /proc/$(pidof minishell)/fd` (Linux).

## Pistes d'extensions
- Support des **guillemets** et de l’**échappement** (`"..."`, `'...'`, `\`).
- **Multiples pipes** (`a | b | c`), **redirections combinées** (`2>`), **&** (arrière-plan).
- Variables d’environnement (`$HOME`, `$PATH`) et substitution.
- Historique des commandes et édition de ligne (readline).
