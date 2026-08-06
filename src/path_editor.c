#include "raylib.h"
#include <asm-generic/errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define VIRTUAL_WIDTH 640
#define VIRTUAL_HEIGHT 480
#define FPS 60

int main() {
  SetTargetFPS(FPS);
  InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "TOWER GAME");

    float path_timer = 0.0f;
  Vector2 path[4000];
  int path_length = 0;

  while (!WindowShouldClose()) {

    // Path drawing tool
      BeginDrawing();
      ClearBackground(BLACK);

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        path_length = 0;
      }

      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        path_timer -= GetFrameTime();

        if (path_timer <= 0.0f) {
          path[path_length++] = GetMousePosition();
          path_timer = 1.0f / 30.0f;
        }
      }

      for (int i = 0; i < path_length - 1; i++) {
        DrawLineEx(path[i], path[i + 1], 20.0f, BROWN);
      }

      if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        printf("Vector2 path[] = {\n}");

        for (int i = 0; i < path_length; i++) {
          printf("    {%0.0ff, %0.0ff},\n", path[i].x, path[i].y);
        }

        printf("};\n");
        printf("int path_length = %d;\n", path_length);
      }
      EndDrawing();
      continue;
    }
}