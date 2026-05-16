#!/usr/bin/env bash
set -e

mkdir -p ~/.apps/rairen/rensa/templates/

cd templates
find . -type f -name '*.rsmk' -exec cp --parents {} ~/.apps/rairen/rensa/templates/ \;

grep -qxF 'export PATH="$HOME/.local/bin:$PATH"' ~/.bashrc || \
  echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
