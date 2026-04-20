set -euo pipefail

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 \"commit message\" <file...>" >&2
  exit 1
fi

msg="$1"
shift

git add "$@"

# staged changes check
git diff --cached --quiet && {
  echo "No changes to commit" >&2
  exit 1
}

git commit -m "$msg"

git push codeberg HEAD
git push github HEAD
