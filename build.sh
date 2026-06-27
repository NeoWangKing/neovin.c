#!/bin/sh

CFLAGS="-O3 -Wall -Wextra"
LIBS="-lm"

source ~/opt/raylib.env > /dev/null 2>&1

CURDIR=$(pwd)
cd "$HOME/emsdk" > /dev/null 2>&1
. ./emsdk_env.sh > /dev/null 2>&1
cd "$CURDIR" > /dev/null 2>&1

UNAME_S=$(uname -s)
if [ "$UNAME_S" = "Darwin" ]; then
    # macOS 专用链接选项
    RL_LIBS="$LIBS $(pkg-config --libs raylib) -framework CoreFoundation -framework CoreGraphics -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL"
elif [ "$UNAME_S" = "Linux" ]; then
    # Linux 专用链接选项
    RL_LIBS="$LIBS $(pkg-config --libs raylib) -lX11 -lXrandr -lXi -lXcursor -lXinerama -lGL -lpthread -ldl"
else
    echo "Unsupported OS: $UNAME_S"
    exit 1
fi

build_NVC() {
  NAME=$1

  # 终端版本
  gcc $CFLAGS -I. -DPLATFORM=TERM_PLATFORM -o ./bin/$NAME.term $NAME.c -lm

  # Raylib 图形版本
  gcc $CFLAGS -I. -DPLATFORM=RAYLIB_PLATFORM `pkg-config --cflags raylib` \
      -o ./bin/$NAME.rl $NAME.c $RL_LIBS

  # WASM 版本
  emcc -I. -DPLATFORM=WASM_PLATFORM \
      -o ./bin/$NAME.wasm $NAME.c \
      -s STANDALONE_WASM=1 \
      -s EXPORTED_FUNCTIONS='["_render","_malloc","_free"]' \
      --no-entry \
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
build_NVC 3d_project/square &
wait
