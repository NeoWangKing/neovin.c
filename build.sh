#!/bin/sh

set -xe

CFLAGS="-O3 -Wall -Wextra"
LIBS="-lm"

mkdir -p ./bin/

gcc $CFLAGS -o ./bin/test test.c $LIBS -lm
./bin/test

source ~/opt/raylib.env
CURDIR=$(pwd)
cd "$HOME/emsdk"
. ./emsdk_env.sh
cd "$CURDIR"

gcc $CFLAGS -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib` -o ./bin/triangle triangle.c $LIBS `pkg-config --libs raylib` -lm -framework CoreFoundation -framework CoreGraphics -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
emcc -DPLATFORM=WASM_PLATFORM triangle.c -o ./bin/triangle.wasm -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' --no-entry -lm

gcc $CFLAGS -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib` -o ./bin/3d 3d.c $LIBS `pkg-config --libs raylib` -lm -framework CoreFoundation -framework CoreGraphics -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
emcc -DPLATFORM=WASM_PLATFORM 3d.c -o ./bin/3d.wasm -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' --no-entry -lm

gcc $CFLAGS -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib` -o ./bin/squish squish.c $LIBS `pkg-config --libs raylib` -lm -framework CoreFoundation -framework CoreGraphics -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
emcc -DPLATFORM=WASM_PLATFORM squish.c -o ./bin/squish.wasm -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' --no-entry -lm

