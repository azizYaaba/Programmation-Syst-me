# TP — Benchmarking et optimisation en programmation système

## 🎯 Objectifs
- Observer l’impact des appels système sur la performance.
- Comparer différentes stratégies de lecture de fichiers : naïve, bufferisée, mmap.
- Utiliser des outils de benchmarking (`time`, `strace`, `perf`).

## 📦 Contenu
- `ex1_naive.c` — lecture caractère par caractère avec `read()`.
- `ex2_buffered.c` — lecture par blocs de 4096 octets.
- `ex3_mmap.c` — lecture avec `mmap`.
- `Makefile` — compilation rapide.

## ⚙️ Compilation
```bash
make
```

## 🧪 Expérimentation

### Générer un gros fichier de test
```bash
dd if=/dev/urandom of=bigfile.bin bs=1M count=100
```

### Lancer les trois versions
```bash
time ./ex1_naive bigfile.bin
time ./ex2_buffered bigfile.bin
time ./ex3_mmap bigfile.bin
```

### Observer les appels système
```bash
strace -c ./ex1_naive bigfile.bin
strace -c ./ex2_buffered bigfile.bin
```

### Profilage avec perf
```bash
perf stat ./ex2_buffered bigfile.bin
```

## 📝 Questions
1. Pourquoi `ex1_naive` est-il plus lent ?
2. Quel est l’impact du nombre d’appels `read()` ?
3. Quels cas d’usage rendent `mmap` particulièrement intéressant ?

