#!/bin/sh

set -xe

mkdir -p ./bin/
gcc -Wall -Wextra -o ./bin/example example.c -lm
# clang -Wall -Wextra --target=wasm32 -o wasm.o -c ./wasm.c
# wasm-ld -m wasm32 --no-entry --export-all --allow-undefined -o wasm.wasm wasm.o

CURDIR=$(pwd)
cd "$HOME/emsdk"
. ./emsdk_env.sh
cd "$CURDIR"

emcc ./wasm.c -o ./bin/wasm.wasm \
    -s STANDALONE_WASM=1 \
    -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' \
    --no-entry \
    -lm

./bin/example
