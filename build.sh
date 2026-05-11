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

  gcc $CFLAGS -I. -DPLATFORM=TERM_PLATFORM -o ./bin/$NAME.term ./examples/$NAME.c
  gcc $CFLAGS -I. -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib`\
    -o ./bin/$NAME.rl ./examples/$NAME.c\
    $LIBS `pkg-config --libs raylib` -lm\
    -framework CoreFoundation\
    -framework CoreGraphics\
    -framework CoreVideo\
    -framework IOKit\
    -framework Cocoa\
    -framework OpenGL
  emcc -I. -DPLATFORM=WASM_PLATFORM\
    -o ./bin/$NAME.wasm ./examples/$NAME.c\
    -s STANDALONE_WASM=1\
    -s EXPORTED_FUNCTIONS='["_render","_get_angle","_malloc","_free"]'\
    --no-entry\
    -lm
}

build_NVC_examples triangle
build_NVC_examples 3d

gcc -I./thirdparty/ -o ./tools/png2c ./tools/png2c.c -lm
./tools/png2c ./neowang.png neowang > neowang.c
./tools/png2c ./amiya.png amiya > amiya.c

build_NVC_examples squish
