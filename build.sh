#!/bin/sh

CFLAGS="-O3 -Wall -Wextra" > /dev/null 2>&1
LIBS="-lm" > /dev/null 2>&1

source ~/opt/raylib.env > /dev/null 2>&1
CURDIR=$(pwd)
cd "$HOME/emsdk" > /dev/null
. ./emsdk_env.sh > /dev/null 2>&1
cd "$CURDIR" > /dev/null

mkdir -p ./bin/

set -xe

gcc $CFLAGS -o ./bin/test test.c $LIBS -lm
./bin/test

gcc $CFLAGS -I. -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib` -o ./bin/triangle ./examples/triangle.c $LIBS `pkg-config --libs raylib` -lm -framework CoreFoundation -framework CoreGraphics -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
emcc -I. -DPLATFORM=WASM_PLATFORM ./examples/triangle.c -o ./bin/triangle.wasm -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' --no-entry -lm

gcc $CFLAGS -I. -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib` -o ./bin/3d ./examples/3d.c $LIBS `pkg-config --libs raylib` -lm -framework CoreFoundation -framework CoreGraphics -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
emcc -I. -DPLATFORM=WASM_PLATFORM ./examples/3d.c -o ./bin/3d.wasm -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' --no-entry -lm

gcc $CFLAGS -I. -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib` -o ./bin/squish ./examples/squish.c $LIBS `pkg-config --libs raylib` -lm -framework CoreFoundation -framework CoreGraphics -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
emcc -I. -DPLATFORM=WASM_PLATFORM ./examples/squish.c -o ./bin/squish.wasm -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' --no-entry -lm

