#!/bin/sh
set -e

./build.sh
./build/rensa $@
