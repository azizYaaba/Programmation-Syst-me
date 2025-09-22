# TP2 – États des processus et ordonnancement

## 🎯 Objectif
Observer la "vie" d'un processus et ses états via `ps` / `top`, et manipuler les **signaux** (`STOP`, `CONT`, `KILL`).

---

## Exercice 1 — Boucle infinie et observation

### But
Créer un processus fils qui s'exécute en continu et observer **PID/PPID**, **état (STAT)**, **priorité**, **%CPU**.

### Code
- `ex1_spin.c` : le père fork un fils qui boucle.
  - Sans argument : boucle CPU (état **R**).
  - Avec `sleep` : boucle avec `sleep(1)` (état **S** majoritaire).

### Compilation & exécution
```bash
make ex1_spin
./ex1_spin           # version "busy"
# ou
./ex1_spin sleep     # version "sleepy"
```

### Observation
- Trouver le PID affiché par le programme (ligne `[FILS] PID=...`).
- Inspecter avec `ps` (Linux/macOS) :
```bash
ps -o pid,ppid,stat,ni,pri,pcpu,cmd -p <PID_FILS>
watch -n 0.5 'ps -o pid,ppid,stat,ni,pri,pcpu,cmd -p <PID_FILS>'
```
- Ouvrir `top`/`htop` pour visualiser l'état :
  - **R** = running (s'exécute)
  - **S** = sleeping (attente)
  - **T** = stopped (arrêté)
  - **Z** = zombie (fini, non réclamé par `wait`)
- Modifier la charge : l'option `sleep` réduit l'utilisation CPU.

---

## Exercice 2 — Signaux : STOP / CONT / KILL

### But
Envoyer des signaux au processus et observer la transition d'état : **R/S → T → R/S → (terminé)**.

### Code
- `ex2_loop.c` : processus qui tourne et affiche un heartbeat.
  - Installe des handlers pour `SIGTERM`/`SIGINT` (démonstration de sortie propre).
  - **NB** : `SIGSTOP` et `SIGKILL` **ne peuvent pas** être interceptés.

### Compilation & exécution
```bash
make ex2_loop
./ex2_loop
# récupérer le PID dans la sortie
```

### Depuis un **autre terminal**
```bash
ps -o pid,ppid,stat,cmd -p <PID>
kill -STOP <PID>   # passe en état T (stopped)
ps -o pid,ppid,stat,cmd -p <PID>
kill -CONT <PID>   # reprise (R ou S)
ps -o pid,ppid,stat,cmd -p <PID>
kill -KILL <PID>   # fin immédiate
ps -p <PID>        # ne devrait plus exister
```

### Optionnel (automatisé)
- `ex2_controller.c` enchaîne STOP → CONT → KILL automatiquement :
```bash
make ex2_controller
./ex2_controller
```
Observe la sortie et confirme via `ps`/`top`.

---

## Notes pédagogiques
- **SIGSTOP/SIGKILL** : impossibles à ignorer/capturer → idéaux pour démontrer l'autorité du noyau.
- **STAT** dans `ps` :
  - `R` : Running
  - `S` : Sleeping (interruptible)
  - `D` : Uninterruptible sleep (I/O)
  - `T` : Stopped (par signal/job control)
  - `Z` : Zombie (le père n'a pas `wait()` le fils)
- **Zombie (bonus)** : si un fils termine et que le père ne fait pas `wait()`, il devient `Z` jusqu'à ce qu'il soit "récolté".
  - À tester en modifiant `ex1_spin.c` pour que le père se termine avant le fils (ou l'inverse) et observer via `ps`.

---

## Débogage
- `strace` (Linux) : `strace -f ./ex2_loop` (voir `nanosleep`, `rt_sigaction`, etc.)
- Affinité/planification (optionnel) : `chrt`, `taskset` pour influences d'ordonnancement.

Bon TP !
