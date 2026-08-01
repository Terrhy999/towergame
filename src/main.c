#include "raylib.h"
#include "raymath.h"
#include <asm-generic/errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

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

// enum {
//     STATE_IDLE,
//     STATE_ATTACKING,
// };

enum {
  PROJECTILE,
  ENEMY,
} ThingType;

typedef struct {
  float x;
  float y;
  float dx;
  float dy;
  int height;
  int width;

  int health;
  float movement_speed;
  float attack_speed;
  float attack_cooldown;
  int damage;

  int attack_radius; // (CG) rename?

  float target_x;
  float target_y;

  Color color;
  int parentind;
} Thing;

#define EMPTY 0
Thing projectiles[2000] = {0};
int projectiles_length = 0;

Thing towers[2000] = {0};
int towers_length = 0;

Thing enemies[2000] = {0};
int enemies_length = 0;

int main() {
  SetTargetFPS(FPS);
  InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "TOWER GAME");

  Thing enemy = {0};
  enemy.height = 25;
  enemy.width = 25;
  enemy.x = VIRTUAL_WIDTH - enemy.height;
  enemy.y = (VIRTUAL_HEIGHT / 2.0f) - enemy.height / 2.0f;
  enemy.health = 10;
  enemy.color = RED;
  enemy.movement_speed = 1;
  enemy.damage = 1;

  Thing heart = {0};
  heart.health = 100;

  Thing tower = {0};
  tower.height = 25;
  tower.width = 25;
  tower.x = VIRTUAL_WIDTH / 2.0f - tower.width / 2.0f;
  tower.y = (VIRTUAL_HEIGHT / 2.0f) - tower.height / 2.0f -
            (50 + tower.height / 2.0f);
  tower.color = BLUE;
  tower.damage = 1;
  tower.attack_speed = 5;
  tower.attack_radius = 150;
  // tower.health = 10;
  // tower.movement_speed = 1;

  // tower attack radius
  // when enemy is inside radius it takes damage
  while (!WindowShouldClose()) {
    tower.attack_cooldown -= GetFrameTime();
    // ============================================================================
    // Input
    // ============================================================================

    // ============================================================================
    // Update
    // ============================================================================

    // Move Projectiles
    for (int i = 0; i < projectiles_length; i++) {
      Thing *projectile = &projectiles[i];

      projectile->x += projectile->dx * projectile->movement_speed;
      projectile->y += projectile->dy * projectile->movement_speed;
    }

    // Check for projectile collision
    for (int i = 0; i < projectiles_length; i++) {
      Thing *projectile = &projectiles[i];

      if (CheckCollisionRecs(
              (Rectangle){projectile->x, projectile->y, projectile->width,
                          projectile->height},
              (Rectangle){enemy.x, enemy.y, enemy.width, enemy.height})) {
        enemy.health = enemy.health - projectile->damage;
        projectiles[i] = projectiles[projectiles_length--];
        i--;
      }
    }

    if (enemy.health <= 0) {
    }

    enemy.x = enemy.x - enemy.movement_speed;
    if (enemy.x == 0) {
      heart.health = heart.health - enemy.damage;
      printf("%i\n", heart.health);
    }

    if (tower.attack_cooldown <= 0.0f &&
        CheckCollisionCircleRec(
            (Vector2){tower.x + (tower.width / 2.0f),
                      tower.y + (tower.height / 2.0f)},
            tower.attack_radius,
            (Rectangle){enemy.x, enemy.y, enemy.width, enemy.height})) {

      tower.attack_cooldown = 1.0f / tower.attack_speed; // (CG)feels wrong
      // Spawn Projectile
      Thing projectile;
      projectile.height = 10;
      projectile.width = 10;
      projectile.x = VIRTUAL_WIDTH / 2.0f - projectile.width / 2.0f;
      projectile.y = (VIRTUAL_HEIGHT / 2.0f) - projectile.height / 2.0f -
                     (50 + projectile.height / 2.0f);
      projectile.color = BLUE;
      projectile.damage = 1;
      //   projectile.attack_speed = 1;
      projectile.attack_radius = 150;
      projectile.movement_speed = 10;
      projectile.color = GRAY;

      projectile.target_x = enemy.x;
      projectile.target_y = enemy.y;

      float distance_x = projectile.target_x - projectile.x;
      float distance_y = projectile.target_y - projectile.y;

      float magnitude =
          sqrtf(distance_x * distance_x + distance_y * distance_y);

      projectile.dx = distance_x / magnitude;
      projectile.dy = distance_y / magnitude;

      projectiles[projectiles_length++] = projectile;
      //   enemy.health -= projectile.damage;
      printf("%i\n", enemy.health);
    }

    // ============================================================================
    //  Drawing
    // ============================================================================
    BeginDrawing();

    DrawRectangle(0, (VIRTUAL_HEIGHT / 2.0f) - 50, VIRTUAL_WIDTH, 100, BROWN);

    DrawCircle(tower.x + tower.width / 2, tower.y + tower.height / 2,
               tower.attack_radius, LIGHTGRAY);
    DrawRectangle(tower.x, tower.y, tower.width, tower.height, tower.color);
    Rectangle tower_rect = {tower.x, tower.y, tower.width, tower.height};
    // DrawRectangleRoundedLines(tower_rect, 2, 4, BLACK);

    DrawRectangle(enemy.x, enemy.y, enemy.width, enemy.height, enemy.color);

    for (int i = 0; i < projectiles_length; i++) {
      Thing projectile = projectiles[i];
      DrawRectangle(projectile.x, projectile.y, projectile.width,
                    projectile.height, projectile.color);
    }

    ClearBackground(BLACK);
    EndDrawing();
  }
  return 0;
}