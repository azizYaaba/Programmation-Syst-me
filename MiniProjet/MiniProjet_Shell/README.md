# TP7 – Mini-projet : Mini-shell avancé

## 🎯 Objectif
Combiner plusieurs notions : **fork/exec**, **pipes (multiples)**, **redirections**, et **gestion de la terminaison** des processus.

## 📁 Contenu
- `minishell_plus.c` — mini-shell interactif avec :
  - Pipeline **multi-étapes** (`a | b | c | ...`)
  - Redirections : `<`, `>`, `>>`, `2>`, `2>>`, `2>&1`
  - Built-ins : `cd`, `exit`
  - Récolte des processus (handler **SIGCHLD**) et attente du pipeline au premier plan
  - Gestion de **SIGINT** (Ctrl-C) : interrompt les enfants, pas le shell

- `Makefile` — compilation rapide.

## ⚙️ Compilation
```bash
make
# ou
gcc -Wall -Wextra -O2 -std=c11 minishell_plus.c -o minishell_plus
```

## ▶️ Exécution
```bash
./minishell_plus
```

## ✅ Exemples à tester
```bash
# 1) Commande simple
ls -l

# 2) Redirections
echo hello > a.txt
cat < a.txt
echo again >> a.txt
grep e < a.txt > out.txt 2> err.txt
grep foo < a.txt 2>&1 | wc -l

# 3) Pipes multiples
cat minishell_plus.c | grep exec | wc -l
ls -l | grep "\.c" | sort | wc -l

# 4) Built-ins
cd ..
cd -        # (non géré ici, extension possible)
exit 0
```

## 📚 Points pédagogiques
- **Pipeline** : composition via `pipe()` et `dup2()`. Chaque étape est forkée et connectée entre elles.
- **Redirections** : `open` + `dup2` pour reconfigurer `stdin/stdout/stderr` avant `execvp()`.
- **Récolte** : éviter les **zombies** via `waitpid` et le handler `SIGCHLD` (utilisé pour des processus en arrière-plan si vous étendez).
- **SIGINT** : `Ctrl-C` remet `SIGINT` par défaut dans les enfants (ils se terminent) ; le shell reste vivant.

## 🧪 Debug / Outils
- `strace -f ./minishell_plus` : tracer `execve`, `pipe`, `dup2`, `wait4`.
- `lsof -p $(pidof minishell_plus)` : vérifier les descripteurs.
- Tester des erreurs : fichier inexistant, permission refusée, commande inconnue (`_exit(127)`).

## 🚀 Extensions (facultatives)
- Guillerets/échappements complets, variables d’environnement, historiques (readline).
- Redirections supplémentaires : `<<` (heredoc), redirections combinées.
- Job control : lancement en arrière-plan `&`, `SIGTSTP`, `fg/bg` (groupes de tâches).

Bon mini-projet !
