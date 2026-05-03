#!/bin/sh

set -xe

mkdir -p ./bin/
gcc -O3 -Wall -Wextra -o ./bin/example example.c -lm

./bin/example
# clang -Wall -Wextra --target=wasm32 -o ./Web/bin/wasm.o -c ./Web/wasm.c
# wasm-ld -m wasm32 --no-entry --export-all --allow-undefined -o ./Web/bin/wasm.wasm ./Web/bin/wasm.o

# CURDIR=$(pwd)
# cd "$HOME/emsdk"
# . ./emsdk_env.sh
# cd "$CURDIR"
#
# emcc ./Web/wasm.c -o ./Web/bin/wasm.wasm \
#     -s STANDALONE_WASM=1 \
#     -s EXPORTED_FUNCTIONS='["_render","_get_angle"]' \
#     --no-entry \
#     -lm
