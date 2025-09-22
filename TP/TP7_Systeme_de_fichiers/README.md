# TP — Gestion des fichiers et systèmes de fichiers

## 🎯 Objectifs
- Comprendre les **appels système POSIX** de manipulation des fichiers : `open`, `read`, `write`, `close`.
- Explorer l'**organisation des systèmes de fichiers** (types de fichiers, arborescence, inodes).
- Maîtriser la **gestion des permissions** et les commandes de sécurité (`chmod`, `chown`, `umask`).

## ⚙️ Pré-requis
- Environnement Linux/WSL avec `gcc`, `make` installés.

## 📦 Contenu
- `ex1_rw.c` — ouverture, écriture et lecture d'un fichier avec API POSIX.
- `ex2_append.c` — écriture en mode ajout (`O_APPEND`).
- `ex3_stat.c` — affichage des métadonnées d’un fichier (taille, inodes, permissions).
- `Makefile` — compilation rapide.
- `README.md` — ce document.

---

## ▶️ Compilation
```bash
make
```

## 🧪 Exercices

### Exercice 1 — Manipulation basique
1. Compiler et lancer `./ex1_rw`.  
2. Observer la création de `demo.txt`.  
3. Modifier le programme pour changer le message écrit.

> ✅ Objectif : comprendre `open`, `write`, `read`, `close`.

---

### Exercice 2 — Mode ajout
1. Lancer `./ex2_append` plusieurs fois.  
2. Observer que les données sont ajoutées sans écraser le fichier.  
3. Comparer avec `O_TRUNC` (tronquer).

> ✅ Objectif : tester différents drapeaux `O_*`.

---

### Exercice 3 — Métadonnées et organisation
1. Compiler et lancer `./ex3_stat fichier`.  
2. Observez : inode, taille, type de fichier, permissions.  
3. Comparez avec les commandes :
```bash
ls -l fichier
stat fichier
```

> ✅ Objectif : comprendre la notion d’**inode** et de métadonnées.

---

### Exercice 4 — Types de fichiers et arborescence
1. Créez un **lien symbolique** et un **lien dur** :  
```bash
ln demo.txt lien_dur.txt
ln -s demo.txt lien_sym.txt
```
2. Comparez avec `ls -li`.  
3. Explorez `/dev` (fichiers spéciaux).

> ✅ Objectif : identifier fichiers réguliers, répertoires, liens, périphériques.

---

### Exercice 5 — Permissions et sécurité
1. Lister les permissions avec `ls -l`.  
2. Modifier avec `chmod` :
```bash
chmod 600 demo.txt
chmod 644 demo.txt
chmod 755 script.sh
```
3. Expérimentez avec `umask` et création de nouveaux fichiers.  
4. Discuter des risques de permissions trop larges (`777`).

> ✅ Objectif : maîtriser les permissions `rwx` (user, group, others).

---

## 📝 Bonnes pratiques
- Toujours **fermer** les fichiers (`close`).  
- Utiliser les **drapeaux adaptés** (`O_RDONLY`, `O_WRONLY`, `O_RDWR`, `O_APPEND`, `O_CREAT`, `O_TRUNC`).  
- Vérifier les retours d’erreurs (`perror`).  
- Protéger les fichiers sensibles avec les bons droits (`chmod`, `umask`).

