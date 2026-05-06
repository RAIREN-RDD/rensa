#!/usr/bin/env bash
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

git push codeberg HEAD &
pid1=$!

git push github HEAD &
pid2=$!

wait $pid1
status1=$?

wait $pid2
status2=$?

if [[ $status1 -ne 0 || $status2 -ne 0 ]]; then
  echo "One or more pushes failed" >&2
  exit 1
fi

echo "Successfully pushed to both remotes"
