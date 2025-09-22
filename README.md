# 🖥️ Programmation Système — Cours & Travaux Pratiques

Bienvenue dans le dépôt du cours **Programmation Système** destiné aux étudiants de Licence en Génie Logiciel.  
Ce dépôt contient **les supports de cours, les TPs pratiques et un mini-projet**, pour apprendre à programmer au plus près du système d’exploitation (Linux, POSIX).

---

## 📚 Contenu du dépôt

### 📖 Cours
- Slides détaillées (PDF)
- Résumés théoriques
- Schémas explicatifs (processus, mémoire, IPC, threads, etc.)

### 🧪 Travaux Pratiques
Chaque TP est livré avec :
- Code source en C (`.c`)
- `Makefile` pour compilation
- `README.md` avec objectifs et consignes

**Liste des TPs :**
1. Découverte des processus (`fork`, `exec`, `wait`)
2. États des processus & signaux (`ps`, `top`, `kill`)
3. Gestion des processus avancée
4. Communication inter-processus (IPC : pipes, files, mémoire partagée)
5. Synchronisation (mutex, sémaphores)
6. Gestion mémoire (`malloc`, `free`, `valgrind`, `mmap`)
7. Mini-projet : **mini-shell POSIX** (processus, pipes, redirections)

### 🚀 Mini-projet
- Implémentation d’un **mini-shell** supportant :
  - exécution de commandes avec `execvp`
  - pipelines `|`
  - redirections (`>`, `<`, `2>`)
  - gestion des signaux & terminaison des processus
- Objectif : combiner toutes les notions du cours.

---

## ⚙️ Installation & Pré-requis

### Environnement recommandé
- **Linux (Ubuntu/Debian/Fedora)** ou **WSL2** sous Windows
- Paquets requis :
  ```bash
  sudo apt update
  sudo apt install build-essential gdb valgrind strace make git
