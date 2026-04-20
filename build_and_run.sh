#!/bin/sh
set -e

./build.sh
docker build -t ephemeral-dev:latest -f lab/Dockerfile .
docker run --rm -it \
  -w /home/dev \
  ephemeral-dev:latest /bin/bash -lc "make -j$(nproc) install -C project && exec /bin/bash"
