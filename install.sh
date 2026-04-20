set -e

mkdir -p ~/.apps/rairen/rensa/templates/

cd templates
find . -type f -name '*.rsmk' -exec cp --parents {} ~/.apps/rairen/rensa/templates/ \;
