# TP3 – Communication entre processus (IPC)

## 🎯 Objectif
Mettre en place la communication **père ↔ fils** via 3 mécanismes IPC : **pipe**, **file de messages System V**, **mémoire partagée + sémaphore**.

## 📁 Contenu
- `ex1_pipe.c` — pipe unidirectionnel (père → fils).
- `ex2_sysv_msgq.c` — file de messages System V (père → fils).
- `ex3_shm_sem.c` — mémoire partagée POSIX + sémaphore (incrément protégé).  
- `Makefile` — compilation rapide.

## ⚙️ Compilation
```bash
make
# ou
gcc -Wall -Wextra -O2 -std=c11 ex1_pipe.c -o ex1_pipe
gcc -Wall -Wextra -O2 -std=c11 ex2_sysv_msgq.c -o ex2_sysv_msgq
gcc -Wall -Wextra -O2 -std=c11 ex3_shm_sem.c -o ex3_shm_sem -pthread
```

## ▶️ Exercice 1 — Pipe
```bash
./ex1_pipe
```
**Attendu :**
```
[PERE] Envoye (.. octets)
[FILS] Recu (...): "Bonjour du père via pipe !"
```

## ▶️ Exercice 2 — File de messages (System V)
```bash
./ex2_sysv_msgq
```
**Attendu :**
```
[FILS] Message recu (type=1): Salut du pere via file de messages !
[PERE] File <id> supprimee.
```
**Outils utiles :**
```bash
ipcs -q       # lister les files
ipcrm -q <id> # supprimer manuellement si besoin
```

## ▶️ Exercice 3 — Mémoire partagée + sémaphore (POSIX)
Partage d'un entier entre père et fils, chacun incrémente `iters` fois sous protection d'un **mutex** (sémaphore binaire).

```bash
./ex3_shm_sem
```
**Attendu :**
```
[PERE] Compteur final attendu=200000, obtenu=200000
```

> Remarque : si vous supprimez la protection (sem_wait/sem_post), vous observerez souvent une valeur **inférieure** (conditions de course).

## 🧩 Notes pédagogiques
- **Pipe** : un canal **unidirectionnel** (close l'extrémité inutilisée !).
- **File de messages System V** : messages **typés** (champ `mtype > 0`). Penser à **supprimer** la file avec `msgctl(..., IPC_RMID, ...)`.
- **Mémoire partagée** : très **rapide**, mais **nécessite** une synchronisation (ici, `sem_t` POSIX avec `pshared=1`).

## 🔧 Débogage
- `strace` (Linux) : `strace -f ./ex1_pipe` / `./ex2_sysv_msgq` / `./ex3_shm_sem`
- Vérifier les ressources System V : `ipcs -q` (queues), `ipcs -m` (shm).
- Erreurs fréquentes : oublier `close` des descripteurs de pipe, ne pas supprimer la msg queue, oublier `-pthread` pour les sémaphores POSIX.

Bon TP !
