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

// Vector2 path[] = {
//     {640, 240}, {500, 240}, {500, 100}, {250, 100}, {250, 350}, {0, 350},
// };
// int path_length = 6;

Vector2 path[] = {
    {628, 39},  {618, 40},  {609, 40},  {599, 40},  {587, 40},  {579, 40},
    {569, 40},  {559, 40},  {548, 40},  {538, 40},  {527, 40},  {517, 40},
    {504, 40},  {495, 40},  {485, 40},  {471, 40},  {463, 40},  {449, 40},
    {439, 40},  {426, 40},  {411, 40},  {401, 40},  {386, 40},  {376, 40},
    {366, 40},  {357, 40},  {347, 40},  {335, 40},  {327, 40},  {316, 40},
    {307, 40},  {298, 40},  {289, 40},  {281, 40},  {269, 40},  {260, 40},
    {251, 40},  {238, 40},  {228, 40},  {214, 40},  {204, 40},  {196, 40},
    {188, 39},  {174, 38},  {166, 38},  {156, 38},  {145, 37},  {134, 37},
    {124, 37},  {115, 37},  {107, 37},  {99, 37},   {91, 37},   {82, 37},
    {75, 41},   {65, 46},   {58, 51},   {52, 57},   {49, 65},   {46, 73},
    {46, 81},   {47, 89},   {52, 96},   {60, 101},  {68, 104},  {77, 107},
    {88, 108},  {100, 110}, {108, 111}, {117, 113}, {130, 115}, {142, 116},
    {152, 118}, {162, 119}, {170, 119}, {178, 120}, {191, 120}, {199, 120},
    {210, 120}, {220, 120}, {228, 120}, {237, 120}, {246, 120}, {255, 120},
    {263, 120}, {271, 120}, {281, 120}, {289, 120}, {297, 120}, {306, 120},
    {316, 121}, {325, 121}, {333, 121}, {341, 121}, {349, 121}, {357, 122},
    {365, 122}, {373, 123}, {384, 126}, {392, 128}, {398, 134}, {402, 141},
    {404, 149}, {406, 157}, {406, 165}, {407, 173}, {407, 181}, {407, 189},
    {407, 197}, {404, 205}, {401, 214}, {395, 221}, {388, 227}, {381, 231},
    {373, 236}, {365, 241}, {357, 246}, {349, 249}, {341, 250}, {333, 253},
    {324, 254}, {315, 254}, {307, 254}, {299, 254}, {292, 250}, {285, 244},
    {283, 236}, {282, 227}, {281, 216}, {281, 207}, {279, 198}, {278, 190},
    {274, 181}, {270, 174}, {263, 168}, {254, 166}, {244, 165}, {236, 164},
    {225, 163}, {217, 164}, {210, 168}, {209, 177}, {211, 187}, {216, 195},
    {216, 203}, {217, 211}, {217, 219}, {216, 227}, {208, 233}, {200, 233},
    {191, 233}, {183, 231}, {172, 225}, {165, 221}, {159, 214}, {155, 206},
    {151, 199}, {148, 191}, {143, 182}, {135, 176}, {125, 174}, {117, 173},
    {109, 174}, {102, 180}, {95, 185},  {88, 192},  {83, 199},  {78, 208},
    {76, 216},  {75, 225},  {75, 233},  {77, 241},  {82, 248},  {86, 256},
    {93, 265},  {97, 272},  {103, 281}, {109, 288}, {117, 296}, {128, 301},
    {141, 307}, {156, 313}, {168, 317}, {179, 320}, {191, 323}, {207, 325},
    {221, 326}, {236, 328}, {245, 328}, {253, 328}, {265, 328}, {279, 331},
    {291, 333}, {305, 334}, {324, 335}, {337, 335}, {348, 335}, {360, 334},
    {376, 331}, {385, 328}, {398, 323}, {411, 317}, {424, 311}, {436, 305},
    {449, 297}, {458, 293}, {466, 286}, {470, 278}, {475, 264}, {476, 255},
    {478, 246}, {478, 232}, {479, 224}, {481, 213}, {482, 205}, {484, 196},
    {487, 183}, {490, 173}, {493, 164}, {498, 155}, {504, 149}, {512, 144},
    {524, 143}, {532, 143}, {539, 148}, {545, 154}, {554, 166}, {560, 175},
    {565, 187}, {569, 196}, {571, 209}, {574, 232}, {576, 244}, {576, 253},
    {576, 263}, {576, 272}, {576, 284}, {576, 293}, {576, 308}, {576, 316},
    {574, 326}, {570, 334}, {566, 341}, {559, 351}, {554, 358}, {547, 364},
    {539, 370}, {530, 375}, {520, 379}, {506, 384}, {497, 386}, {486, 388},
    {477, 390}, {469, 392}, {460, 395}, {452, 397}, {443, 399}, {433, 401},
    {421, 405}, {411, 408}, {401, 409}, {390, 410}, {380, 411}, {370, 412},
    {354, 412}, {344, 412}, {335, 412}, {327, 412}, {317, 412}, {307, 412},
    {297, 412}, {287, 412}, {278, 412}, {266, 412}, {244, 412}, {226, 412},
    {206, 412}, {197, 412}, {182, 412}, {170, 412}, {159, 412}, {142, 412},
    {130, 412}, {121, 411}, {108, 409}, {100, 408}, {91, 407},  {82, 407},
    {69, 407},  {59, 407},  {48, 407},  {39, 407},  {29, 407},  {21, 407},
    {12, 407},  {4, 407},   {-5, 407},
};
int path_length = 297;

float enemy_spawn_timer = 0.0f;

void SpawnEnemy() {
  Thing enemy = {0};

  enemy.x = path[0].x;
  enemy.y = path[0].y;
  enemy.destination = 1;
  enemy.health = 10;
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

void PlaceTower(Vector2 position) {
  Thing tower = {0};
  tower.height = 25;
  tower.width = 25;
  tower.x = position.x - tower.width / 2.0f;
  tower.y = position.y - tower.height / 2.0f;
  tower.color = BLUE;
  tower.damage = 1;
  tower.attack_speed = 5;
  tower.attack_radius = 150;

  towers[towers_length++] = tower;
}

int FindTarget(Thing *tower) {
  // Find the enemy closest to the end, in the radius of this tower
  int closest_enemy_index = -1;
  float closest_enemy_distance = INFINITY;

  for (int i = 0; i < enemies_length; i++) {
    Thing *enemy = &enemies[i];

    if (CheckCollisionCircleRec(
            (Vector2){tower->x + tower->width / 2.0f,
                      tower->y + tower->height / 2.0f},
            tower->attack_radius,
            (Rectangle){enemy->x, enemy->y, enemy->width, enemy->height})) {

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
  return closest_enemy_index;
}

int main() {
  SetTargetFPS(FPS);
  InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "TOWER GAME");

  Thing heart = {0};
  heart.health = 100;

  while (!WindowShouldClose()) {
    // ============================================================================
    // Input
    // ============================================================================

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      PlaceTower(GetMousePosition());
    }

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
        int closest_enemy_index = FindTarget(tower);

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