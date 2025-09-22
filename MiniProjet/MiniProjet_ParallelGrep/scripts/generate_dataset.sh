#!/usr/bin/env bash
set -e
mkdir -p data
# Générer 200 fichiers de 1 Mo avec hasard + quelques occurrences
for i in $(seq -w 000 199); do
  dd if=/dev/urandom of=data/file_$i.bin bs=1M count=1 status=none
done
# Injecter le motif "lorem" dans ~1/3 des fichiers
for f in data/file_0*.bin data/file_1*.bin 2>/dev/null; do
  if (( RANDOM % 3 == 0 )); then
    printf "lorem ipsum dolor sit amet\n" >> "$f"
  fi
done
echo "[OK] Dataset généré dans ./data"
