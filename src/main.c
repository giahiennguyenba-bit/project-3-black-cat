#include "camera.h"
#include "game.h"
#include "map.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 1351
#define SCREEN_HEIGHT 760
#define VIRTUAL_HEIGHT 760
#define VIRTUAL_WIDTH 1351

// Danh sách các bản đồ
const char* mapFiles[] = {
    "assets/tutorial map.tmj",
    "assets/boss1.tmj"
};
int currentMapIndex = 0;
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
  Vector2 spawnPoint = MapGetSpawnPoint(map);
  Player player = {0};
  InitPlayer(&player, spawnPoint);
  player.groundY = MapGetGroundY(map, player.position.x, player.position.y);
  if (player.position.y < player.groundY) {
      player.isJumping = true;
  }
  printf("INITIAL PLAYER Y: %.2f, groundY: %.2f\n", player.position.y, player.groundY);

  Texture2D texIdle = LoadTexture("FREE_Cat 2D Pixel Art/FREE_Cat 2D Pixel Art/Sprites/IDLE.png");
  Texture2D texWalk = LoadTexture("FREE_Cat 2D Pixel Art/FREE_Cat 2D Pixel Art/Sprites/WALK.png");
  Texture2D texRun = LoadTexture("FREE_Cat 2D Pixel Art/FREE_Cat 2D Pixel Art/Sprites/RUN.png");
  Texture2D texJump = LoadTexture("FREE_Cat 2D Pixel Art/FREE_Cat 2D Pixel Art/Sprites/JUMP.png");
  Texture2D texAttack = LoadTexture("FREE_Cat 2D Pixel Art/FREE_Cat 2D Pixel Art/Sprites/ATTACK 1.png");
  Texture2D texTiles = LoadTexture("LAMO/Final/Tiles.png");

  MyCamera myCam = CameraNew(player.position.x, player.position.y, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
  
  myCam.rl.offset.y = VIRTUAL_HEIGHT * 0.72f; 
  CameraSetSmoothDamped(&myCam, 10.0f);
  
  // Thiết lập biên ban đầu và zoom dựa trên map
  float mapW = (float)map->width * map->tilewidth;
  float mapH = (float)map->height * map->tileheight;
  
  if (currentMapIndex == 1) {
      myCam.zoom = 1.95f;
      CameraSetBounds(&myCam, mapW, mapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
      myCam.bounds.y = 280.0f;
      myCam.bounds.height = mapH - 280.0f;
  } else {
      CameraSetBounds(&myCam, mapW, mapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
      float zoomX = (float)VIRTUAL_WIDTH / mapW;
      float zoomY = (float)VIRTUAL_HEIGHT / mapH;
      myCam.zoom = (zoomX > zoomY) ? zoomX : zoomY;
      myCam.zoom += 0.02f;
      if (myCam.zoom < 1.2f) myCam.zoom = 1.2f;
  }

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
            Vector2 spawnPoint = MapGetSpawnPoint(map);
            player.position = spawnPoint;
            player.groundY = MapGetGroundY(map, player.position.x, player.position.y);
            player.velocity = (Vector2){0, 0};
            if (player.position.y < player.groundY) {
                player.isJumping = true;
            } else {
                player.isJumping = false;
            }
            
            // Cập nhật biên camera và zoom cho map mới
            float newMapW = (float)map->width * map->tilewidth;
            float newMapH = (float)map->height * map->tileheight;
            if (currentMapIndex == 1) {
                myCam.zoom = 1.95f;
                CameraSetBounds(&myCam, newMapW, newMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                myCam.bounds.y = 280.0f;
                myCam.bounds.height = newMapH - 280.0f;
            } else {
                CameraSetBounds(&myCam, newMapW, newMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                float zoomX = (float)VIRTUAL_WIDTH / newMapW;
                float zoomY = (float)VIRTUAL_HEIGHT / newMapH;
                myCam.zoom = (zoomX > zoomY) ? zoomX : zoomY;
                myCam.zoom += 0.02f;
                if (myCam.zoom < 1.2f) myCam.zoom = 1.2f;
            }
            
            printf("Switched to: %s (GroundY: %.1f, Zoom: %.2f)\n", mapFiles[currentMapIndex], player.groundY, myCam.zoom);
        }
    }

    // Cập nhật tọa độ đất động tại vị trí X hiện tại của người chơi
    player.groundY = MapGetGroundY(map, player.position.x, player.position.y);

    float oldX = player.position.x;
    UpdatePlayer(&player, dt);
    
    // Kiểm tra va chạm tường ngang (Wall Collision)
    if (MapCheckWallCollision(map, player.position)) {
        player.position.x = oldX;
    }
    
    // Xử lý va chạm trần nhà (Ceiling Collision)
    float ceilingY = MapGetCeilingY(map, player.position.x, player.position.y);
    if (ceilingY != -9999.0f) {
        float headOffset = 36.0f; // Khoảng cách từ chân đến đầu mèo
        if (player.position.y - headOffset < ceilingY) {
            player.position.y = ceilingY + headOffset;
            if (player.velocity.y < 0) {
                player.velocity.y = 0; // Chặn lại không cho bay xuyên trần
            }
        }
    }
    
    // Giới hạn người chơi trong biên của bản đồ (cách biên 16px để không lòi sprite) và chuyển map
    float mapW = (float)map->width * map->tilewidth;
    bool transitioned = false;
    
    if (currentMapIndex == 0 && player.position.x < 16.0f) {
        // Chuyển sang boss1.tmj
        currentMapIndex = 1;
        MapUnload(map);
        map = MapLoad(mapFiles[currentMapIndex]);
        if (map) {
            float newMapW = (float)map->width * map->tilewidth;
            float newMapH = (float)map->height * map->tileheight;
            player.position.x = newMapW - 24.0f; // Điểm spawn phía bên phải map boss1
            player.position.y = MapGetGroundY(map, player.position.x, -9999.0f);
            player.groundY = player.position.y;
            player.velocity = (Vector2){0, 0};
            player.isJumping = false;
            
            // Cập nhật biên camera cho map mới (boss 1)
            CameraSetBounds(&myCam, newMapW, newMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
            myCam.zoom = 1.95f;
            myCam.bounds.y = 280.0f;
            myCam.bounds.height = newMapH - 280.0f;
            
            transitioned = true;
            printf("Transitioned to boss1.tmj! Spawn X: %.1f, Y: %.1f\n", player.position.x, player.position.y);
        }
    } else if (currentMapIndex == 1 && player.position.x > mapW - 16.0f) {
        // Chuyển về tutorial map
        currentMapIndex = 0;
        MapUnload(map);
        map = MapLoad(mapFiles[currentMapIndex]);
        if (map) {
            float newMapW = (float)map->width * map->tilewidth;
            float newMapH = (float)map->height * map->tileheight;
            player.position.x = 24.0f; // Điểm spawn phía bên trái tutorial map
            player.position.y = MapGetGroundY(map, player.position.x, -9999.0f);
            player.groundY = player.position.y;
            player.velocity = (Vector2){0, 0};
            player.isJumping = false;
            
            // Cập nhật biên camera cho map mới
            CameraSetBounds(&myCam, newMapW, newMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
            
            // Cập nhật zoom động cho map mới
            float zoomX = (float)VIRTUAL_WIDTH / newMapW;
            float zoomY = (float)VIRTUAL_HEIGHT / newMapH;
            myCam.zoom = (zoomX > zoomY) ? zoomX : zoomY;
            myCam.zoom += 0.02f;
            if (myCam.zoom < 1.2f) myCam.zoom = 1.2f;
            
            transitioned = true;
            printf("Transitioned back to tutorial map! Spawn X: %.1f, Y: %.1f\n", player.position.x, player.position.y);
        }
    }
    
    if (!transitioned) {
        if (player.position.x < 16.0f) player.position.x = 16.0f;
        if (player.position.x > mapW - 16.0f) player.position.x = mapW - 16.0f;
    }
    
    // Kiểm tra va chạm với bẫy (trap layer)
    Rectangle playerRect = { player.position.x - 12.0f, player.position.y - 36.0f, 24.0f, 36.0f };
    if (MapCheckLayerCollision(map, "trap", playerRect)) {
        if (player.hurtTimer <= 0.0f) {
            player.health -= 1;
            player.hurtTimer = 1.0f; // Bất tử trong 1.0 giây
            player.velocity.y = -500.0f; // Nảy lên nhẹ khi dính bẫy
            CameraShake(&myCam, 0.2f, 5.0f); // Rung màn hình nhẹ
            printf("PLAYER HIT TRAP! HP: %d/%d\n", player.health, player.maxHealth);
        }
    }
    
    // Hồi sinh nếu hết máu
    if (player.health <= 0) {
        Vector2 spawnPoint = MapGetSpawnPoint(map);
        player.position = spawnPoint;
        player.groundY = MapGetGroundY(map, player.position.x, player.position.y);
        player.health = player.maxHealth;
        player.velocity = (Vector2){0, 0};
        player.hurtTimer = 0.0f;
        player.isJumping = false;
        printf("PLAYER DIED! Respawning to spawnpoint: %.1f, %.1f...\n", spawnPoint.x, spawnPoint.y);
    }
    
    Vector2 camTarget = { player.position.x, player.position.y - 86.0f };
    CameraUpdate(&myCam, camTarget, dt);

    BeginTextureMode(target);
    ClearBackground(BLACK);
    BeginMode2D(myCam.rl);

    // Vẽ bản đồ chỉ một lần (không lặp lại)
    cute_tiled_layer_t* layer = map->layers;
    while (layer) {
        if (layer->visible) {
            MapDrawLayer(map, layer->name.ptr, 0.0f, texTiles);
        }
        layer = layer->next;
    }

    DrawPlayer(&player, texIdle, texWalk, texRun, texJump, texAttack, 64, 64, 1.0f);

    EndMode2D();
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height};
    Rectangle destRec = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    DrawTexturePro(target.texture, sourceRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
    
    // Vẽ HP bar (Health bar)
    DrawRectangle(20, 40, 200, 20, DARKGRAY);
    float hpPercent = (float)player.health / (float)player.maxHealth;
    if (hpPercent < 0) hpPercent = 0;
    DrawRectangle(20, 40, (int)(200.0f * hpPercent), 20, RED);
    DrawRectangleLines(20, 40, 200, 20, RAYWHITE);
    DrawText(TextFormat("HP: %d/%d", player.health, player.maxHealth), 30, 42, 16, RAYWHITE);
    
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
  UnloadRenderTexture(target);
  CloseAudioDevice(); // Đóng hệ thống âm thanh
  CloseWindow();
  return 0;
}
