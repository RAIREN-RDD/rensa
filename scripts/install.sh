#!/usr/bin/env bash
set -e

bash scripts/build.sh

mkdir -p ~/.apps/rairen/rensa/templates/

cd resources/templates
find . -type f -name '*.rsmk' -exec cp --parents {} ~/.apps/rairen/rensa/templates/ \;

grep -qxF 'export PATH="$HOME/.local/bin:$PATH"' ~/.bashrc || \
  echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc

cd ../..

cp build/entry ~/.apps/rairen/rensa
mkdir -p ~/.local/bin
ln -sf ~/.apps/rairen/rensa/entry ~/.local/bin/rensa

