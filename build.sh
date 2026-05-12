#!/bin/sh

CFLAGS="-O3 -Wall -Wextra"
LIBS="-lm"

source ~/opt/raylib.env > /dev/null 2>&1

CURDIR=$(pwd)
cd "$HOME/emsdk" > /dev/null 2>&1
. ./emsdk_env.sh > /dev/null 2>&1
cd "$CURDIR" > /dev/null 2>&1

build_NVC() {
  NAME=$1

  gcc $CFLAGS -I. -DPLATFORM=TERM_PLATFORM -o ./bin/$NAME.term $NAME.c
  gcc $CFLAGS -I. -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib`\
    -o ./bin/$NAME.rl $NAME.c\
    $LIBS `pkg-config --libs raylib` -lm\
    -framework CoreFoundation\
    -framework CoreGraphics\
    -framework CoreVideo\
    -framework IOKit\
    -framework Cocoa\
    -framework OpenGL
  emcc -I. -DPLATFORM=WASM_PLATFORM\
    -o ./bin/$NAME.wasm $NAME.c\
    -s STANDALONE_WASM=1\
    -s EXPORTED_FUNCTIONS='["_render","_malloc","_free"]'\
    --no-entry\
    -lm
}

set -xe

mkdir -p ./bin/
gcc $CFLAGS -o ./bin/test test.c $LIBS -lm &
gcc -I./thirdparty/ -o ./tools/png2c ./tools/png2c.c -lm &
wait
./tools/png2c ./imgs/neowang.png neowang > ./imgs/neowang.c &
./tools/png2c ./imgs/amiya.png amiya > ./imgs/amiya.c &
./bin/test &
wait
mkdir -p ./bin/examples/
build_NVC examples/triangle &
build_NVC examples/3d &
build_NVC examples/squish &
wait
mkdir -p ./bin/3d_project/
build_NVC 3d_project/triangles &
build_NVC 3d_project/dots &
wait
