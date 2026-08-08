#!/usr/bin/env bash
set -euo pipefail

cd -- "$(dirname -- "$0")"
mkdir -p build

clang ./src/main.c \
  -I./vendored/raylib/src/ \
  ./vendored/raylib/src/libraylib.a \
  -lglfw -lGL -lopenal -lm -lpthread -ldl -lrt -lX11 \
  -o ./build/towergame

clang ./src/path_editor.c \
-I./vendored/raylib/src/ \
./vendored/raylib/src/libraylib.a \
-lglfw -lGL -lopenal -lm -lpthread -ldl -lrt -lX11 \
-o ./build/path_editor