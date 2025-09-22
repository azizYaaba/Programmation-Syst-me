# TP – Programmation multi-thread (pthreads) — Édition V2

## 🎯 Objectifs
- Créer et gérer des threads POSIX (`pthread_create`, `pthread_join`).
- Comprendre le **partage mémoire** et les **conditions de course**.
- Synchroniser avec **mutex**, **sémaphores**, **barrières**, **variables de condition**.
- Identifier et reproduire **deadlock** et **starvation**, puis les corriger.
- Instrumenter et analyser avec `htop`, `gdb`, `valgrind --tool=helgrind`.

## 📦 Contenu
- `ex1_threads_basics.c` — création/joins, passage d’arguments, retour.
- `ex2_race.c` — **course** volontaire (variable globale non protégée).
- `ex3_mutex.c` — correction avec **mutex** + mesure simple.
- `ex4_prodcons.c` — **producteur/consommateur** (tampon borné) avec **sémaphores + mutex**.
- `ex5_barrier.c` — synchronisation par **barrière** (phases).
- `ex6_deadlock.c` — **deadlock** volontaire sur ordre de verrous.
- `ex7_starvation.c` — **starvation** via boucle de spins et priorités.
- `ex8_condvar.c` — **variables de condition** (`pthread_cond_t`) pour signaler un état.

## ⚙️ Compilation
```bash
make
# ou
gcc -Wall -Wextra -O2 -std=c11 ex6_deadlock.c -o ex6_deadlock -pthread
```

## ▶️ Exécution rapide
```bash
./ex1_threads_basics
./ex2_race 8 2000000
./ex3_mutex 8 2000000
./ex4_prodcons 2 2 50
./ex5_barrier 4 3
./ex6_deadlock
./ex7_starvation 4 2
./ex8_condvar
```

## 🔎 Observation & Debug
- **Voir les threads** : `htop` puis `H` (Linux).
- **GDB** : `gdb ./ex6_deadlock` → `info threads`, `bt`.
- **Helgrind (races)** :
  - `valgrind --tool=helgrind ./ex2_race 8 2000000` (signale des courses).
  - `valgrind --tool=helgrind ./ex3_mutex 8 2000000` (propre).
- **ltrace/strace** utiles pour I/O et syscalls.

## 🧪 Questions guidées
1. Pourquoi `ex2_race` rend un résultat < attendu ? (trace `helgrind`)
2. Comment minimiser la **section critique** dans `ex3_mutex` ?
3. Dans `ex4_prodcons`, retirer le `mutex` mais garder les sémaphores : que se passe-t-il ?
4. `ex6_deadlock` : comment supprimer l’interblocage ? (ordre global de verrous)
5. `ex7_starvation` : comment rendre l’accès plus équitable ? (backoff, yield, condvar)
6. `ex8_condvar` : quel intérêt par rapport au polling ?
