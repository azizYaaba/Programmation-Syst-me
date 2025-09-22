# Mini-projet — Parallel Grep (mmap + threads + FS)

## 🎯 Objectif
Construire un utilitaire **parallèle** qui recherche une **chaîne** dans tous les fichiers d’un répertoire (par défaut récursif), en utilisant :
- **Gestion de la mémoire** : `mmap` pour lire les fichiers efficacement, allocations maîtrisées, évitement des copies.
- **Programmation multi-thread** : pool de threads avec **mutex/condvar**, file de tâches.
- **Système de fichiers** : parcours récursif, ouverture/lecture bas niveau, création d’un fichier résultat avec **permissions contrôlées** (`umask`, `open` + mode 0640).
- (+) **Benchmarking** simple avec `/usr/bin/time`, et observation via `strace -c` et `perf stat` (facultatif).

## 🧩 Fonctionnalités
- Parcourt un dossier (par défaut `.`) et prend en charge les fichiers réguliers.
- Pour chaque fichier, **mmap** le contenu, compte le nombre d’occurrences d’un motif (byte pattern, sensible à la casse).
- Écrit un **rapport CSV** `results.csv` : `path,size_bytes,occurrences`.
- **N threads** travailleurs (par défaut = nombre de CPU).

## ⚙️ Compilation
```bash
make
# ou
gcc -Wall -Wextra -O2 -std=c11 -pthread src/pgrep_parallel.c -o pgrep_parallel
```

## ▶️ Utilisation
```bash
./pgrep_parallel -p "motif" -d ./mon_dossier -t 8 -o results.csv
```
Options :
- `-p <pattern>` : motif à chercher (obligatoire).
- `-d <dir>` : répertoire racine (défaut: `.`).
- `-t <threads>` : nombre de threads (défaut: nb de CPU détecté).
- `-o <output.csv>` : fichier de sortie (défaut: `results.csv`).
- `-x` : ne pas parcourir récursivement (scan non-récursif).

## 🔒 Permissions & sécurité
- Le fichier de sortie est créé avec `umask` et **mode 0640** (`rw- r-- ---`), via `open(..., 0640)`. Ajustez la **umask** si besoin : `umask 027`.
- Ouverture bas niveau : `open`, `write`, `close`. Pas de `fopen`/`fprintf` dans la voie critique.

## 🧪 Benchmark rapide
```bash
scripts/generate_dataset.sh  # génère un jeu de données (~200 Mo) dans ./data
/usr/bin/time -v ./pgrep_parallel -p "lorem" -d ./data -t 8 -o results.csv
strace -c ./pgrep_parallel -p "lorem" -d ./data -t 8 -o results.csv
perf stat ./pgrep_parallel -p "lorem" -d ./data -t 8 -o results.csv
```

## 🔍 Points pédagogiques
- **Réduction des syscalls** via lecture par **mmap**.
- **Localité mémoire** : parcours séquentiel de la zone mappée.
- Synchronisation **mutex + condition variable** pour le pool de threads.
- Gestion d’erreurs robuste : chaque syscall vérifié (`perror`).
- **Nettoyage** : `munmap`, `close`, `free` → pas de fuites.

## 🧱 Structure
```
src/pgrep_parallel.c   # programme principal (pool + FS + mmap + CSV)
Makefile
scripts/generate_dataset.sh
```

## ✅ Extensions possibles
- Ignorer des extensions (binaire vs texte), option `--ignore`.
- Recherche insensible à la casse, motif regex (via PCRE).
- Progression et ETA (barre de progression).
- Mode "producer-only" avec IO thread dédié si disque lent.
