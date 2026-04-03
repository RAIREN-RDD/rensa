#!/bin/sh
set -e

PROJECT_DIR="$(pwd)"
LIB_DIR="$PROJECT_DIR/libs"
mkdir -p "$LIB_DIR"

YAML_CPP_DIR="$PROJECT_DIR/ext/yaml-cpp"
BUILD_DIR="$YAML_CPP_DIR/build"
TARGET="$BUILD_DIR/libyaml-cpp.a"
TAG="yaml-cpp-0.9.0"



if [ ! -f "$TARGET" ]; then
    git submodule update --init --recursive
    cd "$YAML_CPP_DIR"
    CURRENT_TAG=$(git describe --tags --exact-match 2>/dev/null || echo "")
    if [ "$CURRENT_TAG" != "$TAG" ]; then
        git fetch --tags
        git checkout "$TAG"
    fi

    mkdir -p "$BUILD_DIR"
    cmake -S "$YAML_CPP_DIR" -B "$BUILD_DIR"
    cmake --build "$BUILD_DIR" --parallel "$(nproc)"
    cp "$TARGET" "$LIB_DIR/"
fi

cd "$PROJECT_DIR"
make -j$(nproc)
