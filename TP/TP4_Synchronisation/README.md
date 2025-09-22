# TP5 – Synchronisation (mutex et sémaphores)

## 🎯 Objectif
Éviter les **conditions de course** en multithreading avec **pthread_mutex** et **sémaphores POSIX**.

## 📁 Contenu
- `ex1_race.c` — incréments concurrents **sans protection** → résultat incorrect.
- `ex2_mutex.c` — correction avec **pthread_mutex** → résultat exact.
- `ex3_prodcons.c` — **producteur/consommateur** avec tampon borné (sémaphores + mutex).

## ⚙️ Compilation
```bash
make
# ou
gcc -Wall -Wextra -O2 -std=c11 ex1_race.c -o ex1_race -pthread
gcc -Wall -Wextra -O2 -std=c11 ex2_mutex.c -o ex2_mutex -pthread
gcc -Wall -Wextra -O2 -std=c11 ex3_prodcons.c -o ex3_prodcons -pthread
```

## ▶️ Exercice 1 — Condition de course
```bash
./ex1_race 8 2000000
```
Attendu : `Obtenu` **<** `Attendu` (pertes d'incréments). Réessayer avec plus d'itérations si besoin.

## ▶️ Exercice 2 — Mutex (section critique)
```bash
./ex2_mutex 8 2000000
```
Attendu : `Obtenu` **==** `Attendu` (pas de perte).

## ▶️ Exercice 3 — Producteur/Consommateur (sémaphores)
```bash
./ex3_prodcons 2 2 50
```
- `BUF_SIZE` par défaut = 16 (modifiable via `-DBUF_SIZE=...` à la compilation).
- Semaphores non nommés (`sem_init(pshared=0)`) + `pthread_mutex_t` pour protéger le tampon.
- Répartition égale de la charge entre consommateurs (le dernier prend le reste si non divisible).

## 🔎 Notes pédagogiques
- **Condition de course** : operations `counter++` non atomiques (lecture → +1 → écriture) intercalées entre threads.
- **Mutex** : garantie d'exclusion mutuelle ; à **toujours** libérer (unlock) — préférer des blocs courts.
- **Sémaphores** : comptent les ressources disponibles ; `sem_wait`/`sem_post` orchestrent producteurs/consommateurs.
- **Performances** : le mutex série la section critique ; augmenter `iters`/threads pour observer l'effet.

## 🧪 Debug / Analyse
- `time ./ex1_race 8 10000000` vs `time ./ex2_mutex 8 10000000` pour comparer performances.
- `strace -f` (Linux) pour voir `futex` et appels sous-jacents.
- `valgrind --tool=helgrind` pour détecter des races (si disponible).

Bon TP !
