#!/bin/sh
set -e

PROJECT_DIR="$(pwd)"
LIB_DIR="$PROJECT_DIR/libs"

YAML_CPP_DIR="$PROJECT_DIR/third_party/yaml-cpp"
YAML_CPP_BUILD="$YAML_CPP_DIR/build"
YAML_CPP_LIB="$YAML_CPP_BUILD/libyaml-cpp.a"

LZMA_DIR="$PROJECT_DIR/third_party/xz"
LZMA_BUILD="$LZMA_DIR/build"
LZMA_LIB="$LZMA_BUILD/liblzma.a"

LIBARCHIVE_DIR="$PROJECT_DIR/third_party/libarchive"
LIBARCHIVE_BUILD="$LIBARCHIVE_DIR/build"
LIBARCHIVE_LIB="$LIBARCHIVE_BUILD/libarchive/libarchive.a"

mkdir -p "$LIB_DIR"

git submodule update --init --recursive

# -----------------------
# yaml-cpp
# -----------------------
if [ ! -f "$YAML_CPP_LIB" ]; then
    cmake -S "$YAML_CPP_DIR" -B "$YAML_CPP_BUILD"
    cmake --build "$YAML_CPP_BUILD" --parallel "$(nproc)"
fi

cp "$YAML_CPP_LIB" "$LIB_DIR/"

# -----------------------
# xz / liblzma
# -----------------------
if [ ! -f "$LZMA_LIB" ]; then
    cmake -S "$LZMA_DIR" -B "$LZMA_BUILD" \
        -DBUILD_SHARED_LIBS=OFF \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DLZMA_BUILD_TESTS=OFF \
        -DLZMA_BUILD_DOCS=OFF

    cmake --build "$LZMA_BUILD" --parallel "$(nproc)"
fi

cp "$LZMA_LIB" "$LIB_DIR/"

# -----------------------
# libarchive
# -----------------------
if [ ! -f "$LIBARCHIVE_LIB" ]; then
    cmake -S "$LIBARCHIVE_DIR" -B "$LIBARCHIVE_BUILD" \
        -DBUILD_SHARED_LIBS=OFF \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_NETTLE=OFF \
        -DENABLE_OPENSSL=OFF \
        -DENABLE_LZMA=ON \
        -DENABLE_ZSTD=ON \
        -DENABLE_BZip2=OFF \
        -DENABLE_LIBXML2=OFF \
        -DENABLE_EXPAT=OFF

    cmake --build "$LIBARCHIVE_BUILD" --parallel "$(nproc)"
fi

cp "$LIBARCHIVE_LIB" "$LIB_DIR/"

cd "$PROJECT_DIR"

make -j$(nproc)
