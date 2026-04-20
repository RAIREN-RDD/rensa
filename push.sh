set -e

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 \"commit message\" <file...>" >&2
  exit 1
fi

msg="$1"
shift

git add "$@"
git commit -m "$msg"

git push codeberg HEAD
git push --mirror github
