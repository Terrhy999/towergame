#include "raylib.h"
#include "raymath.h"
#include <asm-generic/errno.h>
#include <math.h>
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

  int destination;

  Color color;
} Thing;

Thing projectiles[2000] = {0};
int projectiles_length = 0;

Thing towers[2000] = {0};
int towers_length = 0;

Thing enemies[2000] = {0};
int enemies_length = 0;

Vector2 path[] = {
    {640, 240}, {500, 240}, {500, 100}, {250, 100}, {250, 350}, {0, 350},
};
int path_length = 6;

float enemy_spawn_timer = 0.0f;

void SpawnEnemy() {
  Thing enemy = {0};

  enemy.x = path[0].x;
  enemy.y = path[0].y;
  enemy.destination = 1;
  enemy.health = 25;
  enemy.movement_speed = 1;
  enemy.damage = 1;
  enemy.height = 25;
  enemy.width = 25;
  enemy.color = RED;

  enemies[enemies_length++] = enemy;
}

void SpawnProjectile(Thing *tower, Thing *enemy) {
  Thing projectile;
  projectile.height = 10;
  projectile.width = 10;
  projectile.x = tower->x;
  projectile.y = tower->y;
  projectile.color = BLUE;
  projectile.damage = 1;
  projectile.attack_radius = 150;
  projectile.movement_speed = 10;
  projectile.color = GRAY;

  projectile.target_x = enemy->x;
  projectile.target_y = enemy->y;

  float distance_x = projectile.target_x - projectile.x;
  float distance_y = projectile.target_y - projectile.y;

  float magnitude = sqrtf(distance_x * distance_x + distance_y * distance_y);

  projectile.dx = distance_x / magnitude;
  projectile.dy = distance_y / magnitude;

  projectiles[projectiles_length++] = projectile;
}

int main() {
  SetTargetFPS(FPS);
  InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "TOWER GAME");

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

  towers[towers_length++] = tower;
  // tower.health = 10;
  // tower.movement_speed = 1;

  while (!WindowShouldClose()) {
    // ============================================================================
    // Input
    // ============================================================================

    // ============================================================================
    // Update
    // ============================================================================

    // =========================================
    // Timers
    for (int i = 0; i < towers_length; i++) {
      Thing *tower = &towers[i];
      tower->attack_cooldown -= GetFrameTime();
    }

    // Stagger enemy spawns
    enemy_spawn_timer -= GetFrameTime();
    if (enemy_spawn_timer <= 0.0f) {
      SpawnEnemy();
      enemy_spawn_timer = 1.0f;
    }

    // Move Projectiles
    for (int i = 0; i < projectiles_length; i++) {
      Thing *projectile = &projectiles[i];

      projectile->x += projectile->dx * projectile->movement_speed;
      projectile->y += projectile->dy * projectile->movement_speed;
    }

    // Move Enemies
    for (int i = 0; i < enemies_length; i++) {
      Thing *enemy = &enemies[i];

      Vector2 destination = path[enemy->destination];

      float distance_x = destination.x - enemy->x;
      float distance_y = destination.y - enemy->y;

      float magnitude =
          sqrtf(distance_x * distance_x + distance_y * distance_y);

      enemy->dx = distance_x / magnitude;
      enemy->dy = distance_y / magnitude;

      enemy->x += enemy->dx * enemy->movement_speed;
      enemy->y += enemy->dy * enemy->movement_speed;

      // Update enemy's next destination when it reaches current destination
      if (magnitude <= enemy->movement_speed) {
        enemy->x = destination.x;
        enemy->y = destination.y;
        enemy->destination++;
      }
    }

    // Check for projectile collision
    for (int i = 0; i < projectiles_length; i++) {
      Thing *projectile = &projectiles[i];

      for (int j = 0; j < enemies_length; j++) {
        Thing *enemy = &enemies[j];

        if (CheckCollisionRecs(
                (Rectangle){projectile->x, projectile->y, projectile->width,
                            projectile->height},
                (Rectangle){enemy->x, enemy->y, enemy->width, enemy->height})) {
          enemy->health = enemy->health - projectile->damage;
          projectiles[i] = projectiles[projectiles_length - 1];
          projectiles_length--;
          i--;
        }
      }
    }

    for (int i = 0; i < enemies_length; i++) {
      Thing *enemy = &enemies[i];

      // Lose health when enemies reach the end of the path
      if (enemy->destination == path_length) {
        heart.health = heart.health - enemy->damage;

        // Have enemies die when they reach the end of path
        enemies[i] = enemies[enemies_length - 1];
        enemies_length--;
        i--;

        printf("%i\n", heart.health);
      }

      // Have enemies die when they reach 0 health
      if (enemy->health <= 0) {
        enemies[i] = enemies[enemies_length - 1];
        enemies_length--;
        i--;
      }
    }

    for (int i = 0; i < towers_length; i++) {
      Thing *tower = &towers[i];
      // Have tower shoot projectile if enemy is in range
      if (tower->attack_cooldown <= 0.0f) {

        // Find the enemy closest to the end, in the radius of this tower
        int closest_enemy_index = -1;
        float closest_enemy_distance = INFINITY;

        for (int i = 0; i < enemies_length; i++) {
          Thing *enemy = &enemies[i];

          if (CheckCollisionCircleRec(
                  (Vector2){tower->x + tower->width / 2.0f,
                            tower->y + tower->height / 2.0f},
                  tower->attack_radius,
                  (Rectangle){enemy->x, enemy->y, enemy->width,
                              enemy->height})) {

            float distance_x = enemy->x - path[enemy->destination].x;
            float distance_y = enemy->y - path[enemy->destination].y;

            float magnitude =
                sqrtf(distance_x * distance_x + distance_y * distance_y);

            if (closest_enemy_index == -1) {
              closest_enemy_index = i;
              closest_enemy_distance = magnitude;
            }

            if (enemy->destination > enemies[closest_enemy_index].destination) {
              closest_enemy_index = i;
              closest_enemy_distance = magnitude;
            } else if (enemy->destination ==
                           enemies[closest_enemy_index].destination &&
                       magnitude < closest_enemy_distance) {
              closest_enemy_index = i;
              closest_enemy_distance = magnitude;
            }
          }
        }

        if (closest_enemy_index != -1) {
          tower->attack_cooldown = 1.0 / tower->attack_speed;
          SpawnProjectile(tower, &enemies[closest_enemy_index]);
          printf("%i\n", enemies[closest_enemy_index].health);
        }
      }
    }

    // ============================================================================
    //  Drawing
    // ============================================================================
    BeginDrawing();

    // Draw Path
    for (int i = 0; i < path_length - 1; i++) {
      DrawLineEx(path[i], path[i + 1], 40.0f, BROWN);
    }

    // Draw Radius
    for (int i = 0; i < towers_length; i++) {
      Thing *tower = &towers[i];
      DrawCircle(tower->x + tower->width / 2.0f,
                 tower->y + tower->height / 2.0f, tower->attack_radius,
                 LIGHTGRAY);
    }

    // Draw Tower
    for (int i = 0; i < towers_length; i++) {
      Thing *tower = &towers[i];
      DrawRectangle(tower->x, tower->y, tower->width, tower->height,
                    tower->color);
    }

    // Draw Enemies
    for (int i = 0; i < enemies_length; i++) {
      Thing *enemy = &enemies[i];
      DrawRectangle(enemy->x, enemy->y, enemy->width, enemy->height,
                    enemy->color);
    }

    // Draw Projectiles
    for (int i = 0; i < projectiles_length; i++) {
      Thing *projectile = &projectiles[i];
      DrawRectangle(projectile->x, projectile->y, projectile->width,
                    projectile->height, projectile->color);
    }

    ClearBackground(BLACK);
    EndDrawing();
  }
  return 0;
}