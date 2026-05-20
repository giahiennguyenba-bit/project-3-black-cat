#include "camera.h"
#include "game.h"
#include "map.h"
#include "boss.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define VIRTUAL_HEIGHT 760
#define VIRTUAL_WIDTH 1351

// Danh sách các bản đồ
const char* mapFiles[] = {
    "assets/back1.tmj",
    "assets/back.tmj"
};
int currentMapIndex = 1;
const int totalMaps = 2;

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Forest - Press 'R' to Switch Maps");
  InitAudioDevice(); // Kích hoạt hệ thống âm thanh (Rất quan trọng cho Vibecoding)
  SetTargetFPS(60);

  RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
  SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

  // 1. Load Bản đồ ban đầu
  cute_tiled_map_t* map = MapLoad(mapFiles[currentMapIndex]);
  if (!map) return -1;

  // 2. Khởi tạo Nhân vật và Camera
  float groundY = MapGetGroundY(map);
  Player player = {0};
  InitPlayer(&player, (Vector2){100, groundY});

  Texture2D texIdle = LoadTexture("assets/cat_png/Cat-png/CAT-IDLE.png");
  Texture2D texWalk = LoadTexture("assets/cat_png/Cat-png/CAT-WALK.png");
  Texture2D texRun = LoadTexture("assets/cat_png/Cat-png/CAT-RUN.png");
  Texture2D texJump = LoadTexture("assets/cat_png/Cat-png/CAT-JUMP.png");
  Texture2D texAttack = LoadTexture("assets/cat_png/Cat-png/CAT-ATTACK.png");
  Texture2D texTiles = LoadTexture("LAMO/Final/Tiles.png");

  MyCamera myCam = CameraNew(player.position.x, player.position.y, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
  myCam.zoom = 1.2f;
  myCam.rl.offset.y = VIRTUAL_HEIGHT * 0.72f; 
  CameraSetSmoothDamped(&myCam, 10.0f);
  
  // Thiết lập biên ban đầu
  CameraSetBounds(&myCam, (float)map->width * map->tilewidth, (float)map->height * map->tileheight, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

  Boss boss;
  InitBoss(&boss, (Vector2){600, groundY});

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // --- LOGIC CHUYỂN MAP ---
    if (IsKeyPressed(KEY_R)) {
        currentMapIndex = (currentMapIndex + 1) % totalMaps;
        
        // Giải phóng map cũ
        MapUnload(map);
        
        // Nạp map mới
        map = MapLoad(mapFiles[currentMapIndex]);
        if (map) {
            // Nhân vật thích ứng với map mới
            player.groundY = MapGetGroundY(map);
            player.position.y = player.groundY; // Đưa mèo lên mặt đất mới
            player.velocity.y = 0;
            player.isJumping = false;
            
            // Cập nhật biên camera cho map mới
            float newMapW = (float)map->width * map->tilewidth;
            float newMapH = (float)map->height * map->tileheight;
            CameraSetBounds(&myCam, newMapW, newMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
            
            printf("Switched to: %s (GroundY: %.1f)\n", mapFiles[currentMapIndex], player.groundY);
        }
    }

    UpdatePlayer(&player, dt);
    UpdateBoss(&boss, &player, &myCam, dt);
    
    Vector2 camTarget = { player.position.x, player.position.y - 86.0f };
    CameraUpdate(&myCam, camTarget, dt);

    BeginTextureMode(target);
    ClearBackground(BLACK);
    BeginMode2D(myCam.rl);

    float mapW = (float)map->width * map->tilewidth;
    int currentChunk = (int)(player.position.x / mapW);

    for (int chunkOffset = -1; chunkOffset <= 1; chunkOffset++) {
      float offsetX = (currentChunk + chunkOffset) * mapW;
      
      cute_tiled_layer_t* layer = map->layers;
      while (layer) {
          if (layer->visible) {
              MapDrawLayer(map, layer->name.ptr, offsetX, texTiles);
          }
          layer = layer->next;
      }
    }

    DrawPlayer(&player, texIdle, texWalk, texRun, texJump, texAttack, 64, 64, 1.0f);
    DrawBoss(&boss, 2.0f);

    EndMode2D();
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height};
    Rectangle destRec = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    DrawTexturePro(target.texture, sourceRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
    
    DrawBossHP(&boss);
    DrawText(TextFormat("MAP: %s  (Press 'R' to Switch)", mapFiles[currentMapIndex]), 20, 80, 20, RAYWHITE);
    DrawFPS(10, 10);
    EndDrawing();
  }

  UnloadTexture(texIdle);
  UnloadTexture(texWalk);
  UnloadTexture(texRun);
  UnloadTexture(texJump);
  UnloadTexture(texAttack);
  UnloadTexture(texTiles);
  MapUnload(map);
  UnloadBossResources();
  UnloadRenderTexture(target);
  CloseAudioDevice(); // Đóng hệ thống âm thanh
  CloseWindow();
  return 0;
}
