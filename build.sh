#!/bin/sh

set -xe

CFLAGS="-O3 -Wall -Wextra"
LIBS="-lm"

mkdir -p ./bin/
gcc $CFLAGS -o ./bin/example example.c $LIBS -lm
./bin/example

# source ~/opt/raylib.env
# gcc $CFLAGS `pkg-config --cflags raylib` -o ./bin/triangle triangle.c $LIBS `pkg-config --libs raylib` -lm\
#     -framework CoreFoundation \
#     -framework CoreGraphics \
#     -framework CoreVideo \
#     -framework IOKit \
#     -framework Cocoa \
#     -framework OpenGL
# gcc -DRAYLIB_PLATFORM $CFLAGS `pkg-config --cflags raylib` -o ./bin/3d 3d.c $LIBS `pkg-config --libs raylib` -lm\
#     -framework CoreFoundation \
#     -framework CoreGraphics \
#     -framework CoreVideo \
#     -framework IOKit \
#     -framework Cocoa \
#     -framework OpenGL
#
# CURDIR=$(pwd)
# cd "$HOME/emsdk"
# . ./emsdk_env.sh
# cd "$CURDIR"
#
# emcc 3d.c -o ./bin/3d.wasm \
#     -s STANDALONE_WASM=1 \
#     -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' \
#     --no-entry \
#     -lm
