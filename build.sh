#!/bin/sh

CFLAGS="-O3 -Wall -Wextra"
LIBS="-lm"

source ~/opt/raylib.env > /dev/null 2>&1

CURDIR=$(pwd)
cd "$HOME/emsdk" > /dev/null 2>&1
. ./emsdk_env.sh > /dev/null 2>&1
cd "$CURDIR" > /dev/null 2>&1

mkdir -p ./bin/

set -xe

gcc $CFLAGS -o ./bin/test test.c $LIBS -lm
./bin/test

build_NVC_examples() {
  NAME=$1

  gcc $CFLAGS -I. -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib` -o ./bin/$NAME ./examples/$NAME.c $LIBS `pkg-config --libs raylib` -lm -framework CoreFoundation -framework CoreGraphics -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
  emcc -I. -DPLATFORM=WASM_PLATFORM ./examples/$NAME.c -o ./bin/$NAME.wasm -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' --no-entry -lm
}

build_NVC_examples triangle
build_NVC_examples 3d
build_NVC_examples squish
