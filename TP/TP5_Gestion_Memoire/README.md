# TP6 – Gestion mémoire

## 🎯 Objectif
Comprendre l'allocation dynamique (`malloc`, `free`), diagnostiquer les erreurs mémoire (fuites), et utiliser `mmap` pour mapper un fichier.

## 📁 Contenu
- `ex1_malloc.c` — allouer un tableau, remplir, afficher, libérer.
- `ex2_leak.c` — provoquer des **fuites mémoire** et les détecter avec **valgrind**.
- `ex3_mmap.c` — mapper un **fichier** en mémoire et afficher une **hexdump** des premiers octets.
- `Makefile` — compilation rapide.

## ⚙️ Compilation
```bash
make
# ou
gcc -Wall -Wextra -O2 -std=c11 ex1_malloc.c -o ex1_malloc
gcc -Wall -Wextra -O2 -std=c11 ex2_leak.c -o ex2_leak
gcc -Wall -Wextra -O2 -std=c11 ex3_mmap.c -o ex3_mmap
```

## ▶️ Exercice 1 — malloc/free
```bash
./ex1_malloc 20
```
- Affiche les 50 premiers éléments (ou moins si n<50), puis **libère** le bloc.
- À modifier pour tester des tailles plus grandes et observer le comportement (erreurs `malloc` si taille trop grande).

## ▶️ Exercice 2 — Fuite mémoire + valgrind
```bash
./ex2_leak
valgrind --leak-check=full --show-leak-kinds=all ./ex2_leak
```
**Attendu (exemple)** :
```
==xxxx== definitely lost: 1,024 bytes in 1 blocks
==xxxx== indirectly lost: 0 bytes in 0 blocks
==xxxx== possibly lost: 40,960 bytes in 10 blocks
...
```
- Expliquez la différence entre **definitely**, **indirectly**, **possibly** lost.
- Variante : corrigez la fuite en ajoutant les `free()` appropriés et comparez les rapports.

## ▶️ Exercice 3 — mmap
Préparez un fichier texte :
```bash
echo "Hello mmap!" > data.txt
./ex3_mmap data.txt
# ou afficher davantage d'octets
./ex3_mmap data.txt 512
```
- Le programme mappe le fichier en lecture et affiche une **hexdump** (offsets, hex, ASCII).
- À essayer avec des fichiers binaires (images, exécutables) pour voir la différence d'affichage.

## 🔎 Notes pédagogiques
- `malloc` peut renvoyer `NULL` → **toujours** vérifier.
- Oublier `free` ⇒ **fuite mémoire** (visible avec Valgrind, massif sur long runtime).
- `mmap` permet un accès **mémoire** aux fichiers (zéro copie à la lecture séquentielle), utile pour de gros fichiers.

## 🧪 Debug / Outils
- `valgrind --track-origins=yes` pour tracer l'origine des accès mémoire invalides (si vous en ajoutez).
- `hexdump -C data.txt` (outil système) pour comparer avec la sortie du programme.
- `ulimit -v` / `ulimit -m` (Linux) pour limiter la mémoire et observer les erreurs d'allocation.

Bon TP !
