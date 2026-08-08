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

#define VIRTUAL_WIDTH 1280
#define VIRTUAL_HEIGHT 960
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
    {1279, 89},  {1271, 89},  {1260, 89},  {1251, 89},  {1242, 89},
    {1233, 89},  {1225, 89},  {1217, 88},  {1208, 88},  {1200, 88},
    {1190, 88},  {1180, 86},  {1172, 86},  {1160, 86},  {1150, 86},
    {1142, 86},  {1132, 86},  {1124, 86},  {1115, 86},  {1106, 86},
    {1097, 86},  {1088, 85},  {1079, 85},  {1067, 84},  {1059, 83},
    {1050, 82},  {1039, 82},  {1031, 82},  {1019, 82},  {1011, 82},
    {1002, 82},  {992, 82},   {980, 82},   {970, 83},   {960, 83},
    {951, 84},   {943, 84},   {935, 85},   {926, 86},   {917, 86},
    {907, 86},   {899, 86},   {891, 86},   {880, 86},   {872, 86},
    {863, 86},   {852, 86},   {841, 86},   {830, 86},   {818, 86},
    {807, 85},   {792, 84},   {782, 83},   {772, 83},   {760, 82},
    {746, 81},   {733, 79},   {725, 79},   {717, 79},   {709, 79},
    {697, 79},   {685, 79},   {677, 79},   {669, 79},   {660, 79},
    {650, 80},   {642, 80},   {634, 80},   {624, 81},   {616, 82},
    {606, 84},   {595, 85},   {584, 86},   {572, 86},   {563, 86},
    {553, 87},   {542, 88},   {534, 88},   {525, 88},   {513, 88},
    {502, 88},   {490, 86},   {479, 85},   {466, 83},   {458, 82},
    {450, 81},   {441, 80},   {428, 79},   {416, 79},   {404, 78},
    {390, 78},   {376, 78},   {365, 78},   {352, 78},   {342, 79},
    {334, 80},   {324, 81},   {315, 82},   {307, 83},   {299, 84},
    {290, 85},   {282, 86},   {273, 86},   {263, 87},   {252, 87},
    {242, 89},   {232, 90},   {224, 92},   {212, 94},   {204, 95},
    {192, 97},   {183, 99},   {173, 102},  {164, 105},  {157, 109},
    {150, 115},  {144, 121},  {138, 127},  {132, 134},  {127, 145},
    {123, 156},  {120, 164},  {116, 171},  {113, 180},  {111, 191},
    {109, 199},  {108, 207},  {107, 216},  {107, 224},  {106, 234},
    {105, 243},  {104, 253},  {102, 261},  {101, 274},  {100, 284},
    {99, 295},   {99, 305},   {99, 316},   {99, 327},   {99, 338},
    {99, 348},   {98, 358},   {97, 366},   {96, 374},   {96, 385},
    {96, 393},   {96, 403},   {96, 413},   {94, 424},   {94, 434},
    {94, 443},   {94, 452},   {94, 460},   {94, 470},   {95, 478},
    {98, 487},   {102, 494},  {108, 500},  {115, 507},  {122, 511},
    {130, 513},  {138, 513},  {149, 512},  {158, 511},  {167, 509},
    {176, 507},  {183, 502},  {188, 494},  {190, 486},  {192, 476},
    {193, 467},  {194, 458},  {194, 449},  {195, 440},  {197, 431},
    {197, 420},  {198, 408},  {198, 399},  {198, 388},  {198, 377},
    {198, 365},  {198, 356},  {199, 343},  {201, 334},  {201, 326},
    {203, 318},  {204, 310},  {205, 301},  {208, 292},  {211, 283},
    {213, 275},  {218, 267},  {222, 260},  {226, 251},  {230, 244},
    {236, 237},  {243, 233},  {254, 229},  {264, 226},  {272, 225},
    {280, 225},  {290, 226},  {300, 228},  {308, 230},  {315, 234},
    {322, 239},  {328, 248},  {334, 256},  {338, 267},  {341, 276},
    {344, 284},  {344, 292},  {345, 302},  {345, 315},  {345, 323},
    {345, 335},  {345, 345},  {343, 356},  {342, 367},  {341, 376},
    {339, 385},  {337, 396},  {335, 409},  {335, 421},  {335, 434},
    {335, 442},  {335, 450},  {336, 458},  {336, 468},  {336, 476},
    {337, 487},  {338, 498},  {339, 508},  {340, 516},  {340, 526},
    {340, 535},  {340, 544},  {340, 552},  {341, 561},  {341, 569},
    {341, 577},  {341, 586},  {340, 597},  {340, 605},  {340, 614},
    {340, 626},  {340, 636},  {340, 644},  {340, 652},  {340, 664},
    {340, 672},  {340, 681},  {340, 689},  {340, 701},  {340, 709},
    {340, 717},  {340, 725},  {339, 734},  {339, 742},  {339, 753},
    {340, 762},  {341, 770},  {342, 779},  {344, 790},  {346, 798},
    {350, 806},  {355, 813},  {363, 819},  {374, 825},  {382, 827},
    {392, 827},  {400, 825},  {408, 824},  {416, 819},  {422, 813},
    {428, 807},  {434, 801},  {440, 793},  {446, 783},  {450, 776},
    {454, 769},  {458, 758},  {462, 748},  {466, 736},  {467, 726},
    {468, 715},  {468, 706},  {468, 696},  {468, 687},  {468, 678},
    {466, 668},  {465, 659},  {465, 651},  {465, 639},  {465, 631},
    {465, 621},  {465, 612},  {465, 603},  {465, 595},  {466, 586},
    {467, 577},  {468, 568},  {468, 559},  {468, 550},  {468, 538},
    {468, 525},  {468, 515},  {468, 504},  {468, 496},  {468, 487},
    {467, 479},  {467, 470},  {466, 461},  {466, 452},  {466, 437},
    {466, 428},  {466, 420},  {466, 411},  {466, 402},  {466, 394},
    {466, 384},  {466, 375},  {466, 365},  {467, 356},  {467, 345},
    {468, 336},  {469, 328},  {470, 319},  {471, 310},  {473, 298},
    {476, 287},  {479, 279},  {483, 272},  {491, 267},  {500, 262},
    {507, 258},  {515, 255},  {523, 254},  {531, 252},  {540, 251},
    {549, 249},  {559, 249},  {568, 247},  {577, 247},  {588, 247},
    {601, 247},  {614, 247},  {624, 246},  {634, 246},  {648, 245},
    {661, 245},  {669, 245},  {685, 245},  {693, 245},  {704, 245},
    {721, 244},  {729, 244},  {737, 244},  {748, 244},  {760, 245},
    {768, 246},  {777, 246},  {789, 246},  {800, 246},  {809, 246},
    {817, 246},  {825, 246},  {834, 246},  {844, 246},  {852, 246},
    {864, 247},  {877, 248},  {888, 248},  {896, 248},  {905, 248},
    {916, 248},  {924, 248},  {936, 249},  {945, 249},  {953, 249},
    {964, 250},  {976, 251},  {991, 251},  {1005, 251}, {1018, 254},
    {1028, 256}, {1039, 257}, {1050, 260}, {1059, 261}, {1072, 264},
    {1081, 266}, {1090, 267}, {1104, 269}, {1113, 270}, {1121, 273},
    {1129, 276}, {1136, 282}, {1142, 289}, {1146, 297}, {1150, 306},
    {1151, 317}, {1151, 329}, {1150, 337}, {1146, 345}, {1141, 353},
    {1136, 360}, {1130, 367}, {1123, 372}, {1116, 376}, {1109, 381},
    {1102, 385}, {1095, 389}, {1087, 392}, {1076, 394}, {1068, 395},
    {1058, 397}, {1042, 399}, {1034, 400}, {1026, 401}, {1017, 401},
    {1006, 402}, {998, 403},  {990, 403},  {980, 403},  {970, 404},
    {958, 404},  {949, 404},  {938, 404},  {930, 404},  {921, 404},
    {909, 404},  {900, 404},  {885, 404},  {876, 404},  {863, 404},
    {851, 404},  {835, 404},  {826, 404},  {814, 403},  {805, 402},
    {791, 402},  {776, 400},  {767, 399},  {751, 398},  {743, 398},
    {733, 397},  {725, 397},  {715, 397},  {706, 397},  {698, 397},
    {690, 400},  {681, 405},  {674, 410},  {665, 417},  {658, 422},
    {651, 429},  {644, 437},  {639, 444},  {635, 452},  {630, 461},
    {627, 470},  {624, 480},  {622, 488},  {620, 497},  {618, 507},
    {617, 516},  {617, 525},  {617, 533},  {616, 542},  {616, 552},
    {617, 561},  {620, 570},  {628, 579},  {633, 586},  {641, 593},
    {647, 599},  {655, 603},  {663, 606},  {671, 608},  {681, 610},
    {690, 611},  {698, 613},  {708, 614},  {717, 615},  {729, 616},
    {738, 618},  {757, 622},  {772, 624},  {786, 626},  {796, 627},
    {806, 626},  {816, 624},  {824, 624},  {834, 624},  {842, 624},
    {850, 624},  {860, 624},  {871, 623},  {883, 622},  {891, 621},
    {901, 620},  {918, 620},  {930, 619},  {941, 619},  {955, 619},
    {966, 619},  {976, 619},  {986, 620},  {999, 622},  {1011, 624},
    {1022, 628}, {1030, 630}, {1037, 634}, {1045, 639}, {1052, 645},
    {1058, 653}, {1065, 660}, {1071, 670}, {1076, 680}, {1081, 692},
    {1083, 703}, {1083, 715}, {1083, 726}, {1083, 735}, {1082, 743},
    {1079, 753}, {1074, 761}, {1069, 769}, {1063, 776}, {1052, 786},
    {1046, 792}, {1039, 797}, {1027, 807}, {1020, 811}, {1013, 815},
    {1005, 820}, {993, 824},  {980, 829},  {972, 833},  {960, 837},
    {952, 841},  {944, 843},  {934, 846},  {920, 850},  {912, 851},
    {903, 853},  {892, 853},  {881, 853},  {872, 852},  {864, 851},
    {856, 849},  {846, 849},  {838, 846},  {827, 842},  {816, 834},
    {808, 828},  {800, 820},  {794, 813},  {788, 804},  {783, 796},
    {776, 786},  {768, 774},  {764, 767},  {757, 756},  {749, 744},
    {743, 735},  {738, 728},  {731, 720},  {726, 713},  {718, 708},
    {710, 705},  {702, 703},  {694, 702},  {686, 702},  {675, 702},
    {664, 705},  {651, 707},  {631, 712},  {622, 715},  {614, 720},
    {607, 724},  {600, 731},  {593, 740},  {587, 747},  {582, 757},
    {579, 765},  {576, 773},  {572, 783},  {568, 792},  {565, 800},
    {562, 808},  {558, 816},  {554, 826},  {548, 834},  {544, 841},
    {539, 848},  {533, 855},  {526, 861},  {518, 869},  {510, 876},
    {500, 882},  {488, 888},  {480, 892},  {471, 897},  {459, 901},
    {450, 903},  {440, 904},  {432, 905},  {423, 906},  {413, 908},
    {402, 909},  {392, 910},  {380, 911},  {372, 911},  {362, 911},
    {354, 911},  {341, 911},  {331, 910},  {323, 908},  {315, 905},
    {304, 903},  {296, 900},  {286, 895},  {279, 891},  {271, 885},
    {262, 879},  {255, 872},  {248, 865},  {241, 858},  {232, 848},
    {225, 836},  {219, 825},  {215, 817},  {210, 805},  {204, 791},
    {201, 782},  {195, 765},  {192, 754},  {189, 746},  {188, 735},
    {187, 725},  {184, 715},  {181, 703},  {179, 692},  {175, 683},
    {171, 673},  {166, 666},  {161, 659},  {155, 651},  {148, 644},
    {140, 639},  {133, 634},  {125, 631},  {117, 628},  {109, 626},
    {99, 624},   {89, 622},   {80, 621},   {72, 620},   {64, 620},
    {56, 619},   {48, 619},   {40, 618},   {31, 618},   {18, 618},
    {10, 618},   {2, 619},
};
int path_length = 667;

float enemy_spawn_timer = 0.0f;

void spawn_enemy() {
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

void spawn_projectile(Thing *tower, Thing *enemy) {
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

void place_tower(Vector2 position) {

  bool can_place_tower = true;

  Rectangle tower_rect = {position.x - 25.0f / 2.0f, position.y - 25.0f / 2.0f,
                          25, 25};

  // Check if tower is on top of path
  for (int i = 0; i < path_length; i++) {
    if (CheckCollisionCircleRec(path[i], 20.0f, tower_rect)) {
      can_place_tower = false;
    }
  }

  // Check if tower is on top of another tower
  for (int i = 0; i < towers_length; i++) {
    if (CheckCollisionRecs((Rectangle){towers[i].x, towers[i].y, 25, 25},
                           tower_rect)) {
      can_place_tower = false;
    }
  }

  if (can_place_tower) {

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
}

int find_target(Thing *tower) {
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
      place_tower(GetMousePosition());
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
      spawn_enemy();
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
        int closest_enemy_index = find_target(tower);

        if (closest_enemy_index != -1) {
          tower->attack_cooldown = 1.0 / tower->attack_speed;
          spawn_projectile(tower, &enemies[closest_enemy_index]);
          printf("%i\n", enemies[closest_enemy_index].health);
        }
      }
    }

    // ============================================================================
    //  Drawing
    // ============================================================================
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw Path
    for (int i = 0; i < path_length - 1; i++) {
      DrawCircleV(path[i], 20.0f, BROWN);
      // DrawLineEx(path[i], path[i + 1], 40.0f, BROWN);
    }

    // Draw Radius
    for (int i = 0; i < towers_length; i++) {
      Thing *tower = &towers[i];
      DrawCircle(tower->x + tower->width / 2.0f,
                 tower->y + tower->height / 2.0f, tower->attack_radius,
                 (Color){200, 200, 200, 78});
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
    EndDrawing();
  }
  return 0;
}