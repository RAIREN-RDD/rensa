#!/usr/bin/env bash
set -e

if ! bash scripts/build.sh; then
    exit 1
fi

./build/entry $@
