#include "camera.h"
#include "game.h"
#include "map.h"
#include "boss.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

// --- Cấu trúc hạt bụi Arena ---
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   size;
    Color   color;
    float   life;
    float   maxLife;
} Particle;

#define MAX_PARTICLES 60
Particle particles[MAX_PARTICLES];

void InitParticles(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].position = (Vector2){ (float)(rand() % 2500), (float)(200 + rand() % 500) };
        particles[i].velocity = (Vector2){ (float)((rand() % 40) - 20) / 10.0f, (float)(-(10 + rand() % 30)) / 10.0f };
        particles[i].size = (float)(1 + rand() % 3);
        particles[i].color = Fade(GOLD, (float)(20 + rand() % 60) / 100.0f);
        particles[i].maxLife = (float)(3 + rand() % 5);
        particles[i].life = (float)(rand() % 100) / 100.0f * particles[i].maxLife;
    }
}

void UpdateParticles(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].position.x += particles[i].velocity.x * dt;
        particles[i].position.y += particles[i].velocity.y * dt;
        particles[i].life -= dt;
        particles[i].velocity.x += sinf(particles[i].life * 2.0f) * 0.1f * dt;
        
        if (particles[i].life <= 0) {
            particles[i].position = (Vector2){ (float)(rand() % 2500), 640.0f };
            particles[i].velocity = (Vector2){ (float)((rand() % 40) - 20) / 10.0f, (float)(-(10 + rand() % 30)) / 10.0f };
            particles[i].size = (float)(1 + rand() % 3);
            particles[i].color = Fade(GOLD, (float)(20 + rand() % 60) / 100.0f);
            particles[i].maxLife = (float)(3 + rand() % 5);
            particles[i].life = particles[i].maxLife;
        }
    }
}

void DrawParticles(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        DrawCircleV(particles[i].position, particles[i].size, particles[i].color);
    }
}

// Trạng thái Intro giới thiệu Boss
typedef enum {
    INTRO_WALK_IN = 0,
    INTRO_PAN_TO_BOSS,
    INTRO_SHOW_NAME,
    INTRO_FIGHT
} IntroState;

int main(void) {
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Forest - Boss Altar Arena");
  InitAudioDevice(); // Kích hoạt hệ thống âm thanh
  SetTargetFPS(60);

  RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
  SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

  // 1. Load Bản đồ ban đầu
  cute_tiled_map_t* map = MapLoad(mapFiles[currentMapIndex]);
  if (!map) return -1;

  InitParticles();

  // 2. Khởi tạo Nhân vật và Camera
  Vector2 spawnPoint = MapGetSpawnPoint(map);
  Player player = {0};
  InitPlayer(&player, spawnPoint);
  player.groundY = MapGetGroundY(map, player.position.x, player.position.y);
  if (player.position.y < player.groundY) {
      player.isJumping = true;
  }
  player.controlsEnabled = true; // Bắt đầu ở tutorial map thì tự do di chuyển

  Texture2D texIdle = LoadTexture("assets/cat_png/Cat-png/CAT-IDLE.png");
  Texture2D texWalk = LoadTexture("assets/cat_png/Cat-png/CAT-WALK.png");
  Texture2D texRun = LoadTexture("assets/cat_png/Cat-png/CAT-RUN.png");
  Texture2D texJump = LoadTexture("assets/cat_png/Cat-png/CAT-JUMP.png");
  Texture2D texAttack = LoadTexture("assets/cat_png/Cat-png/CAT-ATTACK.png");
  Texture2D texHurt = LoadTexture("assets/cat_png/Cat-png/CAT-HURT.png");
  Texture2D texTiles = LoadTexture("LAMO/Final/Tiles.png");

  MyCamera myCam = CameraNew(player.position.x, player.position.y, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
  myCam.rl.offset.y = VIRTUAL_HEIGHT * 0.72f; 
  CameraSetSmoothDamped(&myCam, 10.0f);

  // Thiết lập biên ban đầu và zoom dựa trên map
  float mapW = (float)map->width * map->tilewidth;
  float mapH = (float)map->height * map->tileheight;
  CameraSetBounds(&myCam, mapW, mapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
  float zoomX = (float)VIRTUAL_WIDTH / mapW;
  float zoomY = (float)VIRTUAL_HEIGHT / mapH;
  myCam.zoom = (zoomX > zoomY) ? zoomX : zoomY;
  myCam.zoom += 0.02f;
  if (myCam.zoom < 1.2f) myCam.zoom = 1.2f;

  Boss boss;
  bool bossInitialized = false;
  LoadBossResources();

  // --- CẤU HÌNH INTRO CHUYỂN CẢNH ---
  IntroState introState = INTRO_FIGHT; // Bắt đầu ở map hướng dẫn thì không có intro
  float introTimer = 0.0f;

  // Tạo âm thanh chuyển cảnh/sấm sét procedurally
  Sound introSound = { 0 };
  {
      Wave wave = { 0 };
      wave.frameCount = 44100 * 2.5f; // Thời gian 2.5 giây
      wave.sampleRate = 44100;
      wave.sampleSize = 16;
      wave.channels = 1;
      wave.data = malloc(wave.frameCount * sizeof(short));
      if (wave.data) {
          short *samples = (short*)wave.data;
          for (int i = 0; i < wave.frameCount; i++) {
              float t = (float)i / 44100.0f;
              float freq = 40.0f + 90.0f * expf(-3.0f * t); // Tiếng rền giảm dần từ 130Hz -> 40Hz
              float amp = 0.6f * expf(-1.2f * t);          // Biên độ giảm dần theo hàm mũ kép
              float noise = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f; // Nhiễu trắng làm hiệu ứng rè sấm sét
              samples[i] = (short)((sinf(2.0f * PI * freq * t) * 0.7f + noise * 0.3f) * amp * 32767.0f);
          }
          introSound = LoadSoundFromWave(wave);
          UnloadWave(wave);
      }
  }

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // --- PHÍM TẮT RESET TRẬN ĐẤU ('R') ---
    if (IsKeyPressed(KEY_R)) {
        if (currentMapIndex == 1) {
            // Reset lại người chơi ở map Boss (spawn phía bên phải)
            float newMapW = (float)map->width * map->tilewidth;
            player.position.x = newMapW - 24.0f;
            player.position.y = MapGetGroundY(map, player.position.x, -9999.0f);
            player.groundY = player.position.y;
            player.velocity = (Vector2){0, 0};
            player.isJumping = false;
            player.currentHP = player.maxHP;
            player.hurtTimer = 0.0f;
            player.controlsEnabled = false;

            // Reset lại Boss
            InitBoss(&boss, (Vector2){300.0f, MapGetGroundY(map, 300.0f, -9999.0f)});
            bossInitialized = true;

            // Reset lại Intro
            introState = INTRO_WALK_IN;
            introTimer = 0.0f;
            printf("Restarted the Boss Fight in Arena!\n");
        } else {
            // Reset ở map hướng dẫn
            Vector2 sp = MapGetSpawnPoint(map);
            player.position = sp;
            player.groundY = MapGetGroundY(map, player.position.x, player.position.y);
            player.velocity = (Vector2){0, 0};
            player.isJumping = false;
            player.currentHP = player.maxHP;
            player.hurtTimer = 0.0f;
            player.controlsEnabled = true;
            introState = INTRO_FIGHT;
            printf("Reset Player to Tutorial Spawn!\n");
        }
    }

    // --- PHÍM TẮT CHUYỂN NHANH ĐẾN BOSS ARENA ('B') ---
    if (IsKeyPressed(KEY_B)) {
        if (currentMapIndex != 1) {
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
                player.controlsEnabled = false; // Khóa phím khi chạy intro

                // Khởi tạo Boss ở bên trái
                InitBoss(&boss, (Vector2){300.0f, MapGetGroundY(map, 300.0f, -9999.0f)});
                bossInitialized = true;

                // Cập nhật biên camera cho map mới (boss 1)
                myCam.zoom = 1.95f;
                CameraSetBounds(&myCam, newMapW, newMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                myCam.bounds.y = 280.0f;
                myCam.bounds.height = newMapH - 280.0f;

                // Bắt đầu chuỗi Intro giới thiệu Boss
                introState = INTRO_WALK_IN;
                introTimer = 0.0f;
                printf("Teleported to Boss Arena via [B]! Spawn X: %.1f, Y: %.1f. Start Intro.\n", player.position.x, player.position.y);
            }
        }
    }


    // --- CẬP NHẬT TRẠNG THÁI GAME ---
    if (currentMapIndex == 1) {
        UpdateParticles(dt);
    }

    // --- CẬP NHẬT THEO TỪNG MAP ---
    if (currentMapIndex == 0) {
        // Map 0: Tutorial Map
        player.groundY = MapGetGroundY(map, player.position.x, player.position.y);
        
        float oldX = player.position.x;
        UpdatePlayer(&player, dt);
        
        // Va chạm tường
        if (MapCheckWallCollision(map, player.position)) {
            player.position.x = oldX;
        }
        
        // Va chạm trần
        float ceilingY = MapGetCeilingY(map, player.position.x, player.position.y);
        if (ceilingY != -9999.0f) {
            float headOffset = 36.0f;
            if (player.position.y - headOffset < ceilingY) {
                player.position.y = ceilingY + headOffset;
                if (player.velocity.y < 0) player.velocity.y = 0;
            }
        }

        // Va chạm bẫy
        Rectangle playerRect = { player.position.x - 12.0f, player.position.y - 36.0f, 24.0f, 36.0f };
        if (MapCheckLayerCollision(map, "trap", playerRect)) {
            if (player.hurtTimer <= 0.0f) {
                player.currentHP -= 1.0f;
                player.hurtTimer = 1.0f;
                player.velocity.y = -500.0f;
                CameraShake(&myCam, 0.2f, 5.0f);
                printf("PLAYER HIT TRAP! HP: %.0f/%.0f\n", player.currentHP, player.maxHP);
            }
        }

        // Hồi sinh nếu hết máu
        if (player.currentHP <= 0.0f) {
            Vector2 sp = MapGetSpawnPoint(map);
            player.position = sp;
            player.groundY = MapGetGroundY(map, player.position.x, player.position.y);
            player.currentHP = player.maxHP;
            player.velocity = (Vector2){0, 0};
            player.hurtTimer = 0.0f;
            player.isJumping = false;
            printf("PLAYER DIED! Respawning to tutorial spawnpoint...\n");
        }

        // Chuyển sang map Boss nếu đi quá rìa trái (X < 16.0f)
        if (player.position.x < 16.0f) {
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
                player.controlsEnabled = false; // Khóa phím khi chạy intro

                // Khởi tạo Boss ở bên trái
                InitBoss(&boss, (Vector2){300.0f, MapGetGroundY(map, 300.0f, -9999.0f)});
                bossInitialized = true;

                // Cập nhật biên camera cho map mới (boss 1)
                myCam.zoom = 1.95f;
                CameraSetBounds(&myCam, newMapW, newMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                myCam.bounds.y = 280.0f;
                myCam.bounds.height = newMapH - 280.0f;

                // Bắt đầu chuỗi Intro giới thiệu Boss
                introState = INTRO_WALK_IN;
                introTimer = 0.0f;
                printf("Transitioned to boss1.tmj! Spawn X: %.1f, Y: %.1f. Start Intro.\n", player.position.x, player.position.y);
            }
        } else {
            // Giới hạn trong map
            if (player.position.x > mapW - 16.0f) player.position.x = mapW - 16.0f;
        }

        // Camera follow player
        Vector2 camTarget = { player.position.x, player.position.y - 86.0f };
        CameraUpdate(&myCam, camTarget, dt);

    } else if (currentMapIndex == 1) {
        // Map 1: Boss Arena
        // Cập nhật tọa độ đất động tại vị trí X hiện tại của người chơi
        player.groundY = MapGetGroundY(map, player.position.x, player.position.y);

        // --- CẬP NHẬT INTRO & GAMEPLAY STATE ---
        if (introState == INTRO_WALK_IN) {
            // Mèo tự động di chuyển sang trái (do spawn bên phải)
            player.position.x -= 160.0f * dt;
            player.facingRight = false;
            player.isRunning = true;
            player.isSprinting = false;
            player.isJumping = false;
            player.isAttacking = false;
            player.controlsEnabled = false;

            // Cập nhật frame chạy thủ công
            player.frameTimer += dt;
            if (player.frameTimer >= 0.1f) {
                player.frameTimer = 0.0f;
                player.currentFrame++;
            }

            if (player.position.x <= 950.0f) {
                player.position.x = 950.0f;
                player.isRunning = false;
                player.currentFrame = 0;
                player.frameTimer = 0.0f;
                introState = INTRO_PAN_TO_BOSS;
                introTimer = 0.0f;
            }

            // Cập nhật Boss đứng yên thở (Idle)
            if (bossInitialized) {
                boss.state = BOSS_STATE_IDLE;
                boss.frameTimer += dt;
                if (boss.frameTimer >= 0.12f) {
                    boss.frameTimer = 0.0f;
                    boss.currentFrame = (boss.currentFrame + 1) % 8;
                }
            }
        }
        else if (introState == INTRO_PAN_TO_BOSS) {
            introTimer += dt;
            float duration = 2.0f;
            if (introTimer > duration) {
                introState = INTRO_SHOW_NAME;
                introTimer = 0.0f;
                
                // Kích hoạt âm thanh tiếng sấm gầm và rung lắc camera
                PlaySound(introSound);
                CameraShake(&myCam, 1.8f, 7.0f);
            }

            // Tĩnh mèo
            UpdatePlayer(&player, dt);

            // Cập nhật Boss đứng yên thở
            if (bossInitialized) {
                boss.state = BOSS_STATE_IDLE;
                boss.frameTimer += dt;
                if (boss.frameTimer >= 0.12f) {
                    boss.frameTimer = 0.0f;
                    boss.currentFrame = (boss.currentFrame + 1) % 8;
                }
            }
        }
        else if (introState == INTRO_SHOW_NAME) {
            introTimer += dt;
            if (introTimer >= 2.8f) {
                introState = INTRO_FIGHT;
                player.controlsEnabled = true; // Trả lại điều khiển cho người chơi
            }

            UpdatePlayer(&player, dt);

            // Cập nhật Boss đứng yên thở
            if (bossInitialized) {
                boss.state = BOSS_STATE_IDLE;
                boss.frameTimer += dt;
                if (boss.frameTimer >= 0.12f) {
                    boss.frameTimer = 0.0f;
                    boss.currentFrame = (boss.currentFrame + 1) % 8;
                }
            }
        }
        else {
            // Chế độ chơi chính thức (INTRO_FIGHT)
            if (player.currentHP > 0.0f) {
                float oldX = player.position.x;
                UpdatePlayer(&player, dt);

                // Va chạm tường
                if (MapCheckWallCollision(map, player.position)) {
                    player.position.x = oldX;
                }

                // Va chạm trần
                float ceilingY = MapGetCeilingY(map, player.position.x, player.position.y);
                if (ceilingY != -9999.0f) {
                    float headOffset = 36.0f;
                    if (player.position.y - headOffset < ceilingY) {
                        player.position.y = ceilingY + headOffset;
                        if (player.velocity.y < 0) player.velocity.y = 0;
                    }
                }

                // Va chạm bẫy
                Rectangle playerRect = { player.position.x - 12.0f, player.position.y - 36.0f, 24.0f, 36.0f };
                if (MapCheckLayerCollision(map, "trap", playerRect)) {
                    if (player.hurtTimer <= 0.0f) {
                        player.currentHP -= 1.0f;
                        player.hurtTimer = 1.0f;
                        player.velocity.y = -500.0f;
                        CameraShake(&myCam, 0.2f, 5.0f);
                        printf("PLAYER HIT TRAP IN ARENA! HP: %.0f/%.0f\n", player.currentHP, player.maxHP);
                    }
                }

                if (bossInitialized) {
                    UpdateBoss(&boss, &player, &myCam, dt);
                }
            } else {
                // Player chết -> khoá phím và đưa boss về trạng thái IDLE
                player.controlsEnabled = false;
                if (bossInitialized) {
                    boss.state = BOSS_STATE_IDLE;
                    boss.velocity.x = 0;
                    boss.frameTimer += dt;
                    if (boss.frameTimer >= 0.12f) {
                        boss.frameTimer = 0.0f;
                        boss.currentFrame = (boss.currentFrame + 1) % 8;
                    }
                }
            }
        }

        // Chuyển về tutorial map nếu người chơi đi quá rìa phải (X > mapW - 16.0f)
        float currentMapW = (float)map->width * map->tilewidth;
        if (player.position.x > currentMapW - 16.0f) {
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
                player.controlsEnabled = true;

                // Vô hiệu hóa Boss
                bossInitialized = false;

                // Cập nhật biên camera cho map mới
                CameraSetBounds(&myCam, newMapW, newMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                
                // Cập nhật zoom động cho map mới
                float zoomX = (float)VIRTUAL_WIDTH / newMapW;
                float zoomY = (float)VIRTUAL_HEIGHT / newMapH;
                myCam.zoom = (zoomX > zoomY) ? zoomX : zoomY;
                myCam.zoom += 0.02f;
                if (myCam.zoom < 1.2f) myCam.zoom = 1.2f;

                introState = INTRO_FIGHT;
                printf("Transitioned back to tutorial map! Spawn X: %.1f, Y: %.1f\n", player.position.x, player.position.y);
            }
        } else {
            // Giới hạn trong map
            if (player.position.x < 16.0f) player.position.x = 16.0f;
        }

        // Cập nhật Camera Target
        Vector2 camTarget = { player.position.x, player.position.y - 86.0f };
        if (introState == INTRO_PAN_TO_BOSS && bossInitialized) {
            camTarget = (Vector2){ boss.position.x, boss.position.y - 86.0f };
        }
        
        // Thiết lập bounds dựa vào trạng thái Intro
        float currentMapH = (float)map->height * map->tileheight;
        CameraSetBounds(&myCam, currentMapW, currentMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
        myCam.bounds.y = 280.0f;
        myCam.bounds.height = currentMapH - 280.0f;
        
        CameraUpdate(&myCam, camTarget, dt);
    }

    // --- RENDER GAMEPLAY ---
    BeginTextureMode(target);
    ClearBackground(BLACK);

    // --- 1. RENDER BACKGROUND SKYBOX (Screen Space) ---
    DrawRectangleGradientV(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, (Color){10, 11, 20, 255}, (Color){28, 16, 48, 255});
    
    // Mặt trăng khổng lồ phát sáng dịu
    Vector2 moonPos = { 850.0f, 180.0f };
    DrawCircleV(moonPos, 140, Fade((Color){230, 240, 255, 255}, 0.02f));
    DrawCircleV(moonPos, 110, Fade((Color){230, 240, 255, 255}, 0.05f));
    DrawCircleV(moonPos, 90, Fade((Color){230, 240, 255, 255}, 0.12f));
    DrawCircleV(moonPos, 80, (Color){240, 243, 255, 255});
    DrawCircle(moonPos.x - 25, moonPos.y - 15, 12, (Color){222, 226, 242, 255});
    DrawCircle(moonPos.x + 30, moonPos.y + 20, 16, (Color){222, 226, 242, 255});
    DrawCircle(moonPos.x - 10, moonPos.y + 30, 8, (Color){222, 226, 242, 255});

    // --- 2. RENDER WORLD (World Space) ---
    BeginMode2D(myCam.rl);

    if (currentMapIndex == 1) {
        // Chỉ vẽ phế tích cột đá ở Arena để phù hợp với bối cảnh boss
        float pX = myCam.rl.target.x * 0.3f;
        Color ruinsColor = (Color){22, 20, 36, 255};
        Color ruinsTrim = (Color){32, 28, 50, 255};
        for (int i = -5; i < 15; i++) {
            float x = i * 300.0f - pX;
            DrawRectangleRec((Rectangle){ x, 250, 40, 390 }, ruinsColor);
            DrawRectangleRec((Rectangle){ x - 5, 240, 50, 10 }, ruinsTrim);
            DrawRectangleRec((Rectangle){ x - 2, 630, 44, 10 }, ruinsTrim);
            if (i % 2 == 0) {
                DrawRectangleRec((Rectangle){ x - 80, 400, 30, 240 }, ruinsColor);
                DrawRectangleRec((Rectangle){ x - 82, 390, 34, 10 }, ruinsTrim);
            }
        }
    }

    // Vẽ bản đồ Tiled
    cute_tiled_layer_t* layer = map->layers;
    while (layer) {
        if (layer->visible) {
            MapDrawLayer(map, layer->name.ptr, 0.0f, texTiles);
        }
        layer = layer->next;
    }

    if (currentMapIndex == 1) {
        // Hiệu ứng hạt bụi phép vàng trôi lơ lửng
        DrawParticles();
        
        // Vẽ Boss
        if (bossInitialized) {
            DrawBoss(&boss, 2.0f);
        }

        // Vẽ vách ngăn ma thuật biên giới hạn khu vực đấu (Dead Cells style)
        if (introState == INTRO_FIGHT) {
            float barrierAlpha = 0.35f + 0.15f * sinf(GetTime() * 7.0f);
            Color barrierColor = (Color){ 255, 46, 99, (unsigned char)(barrierAlpha * 255) };
            Color glowColor = (Color){ 255, 46, 99, 80 };
            
            // Vách năng lượng bên trái
            DrawRectangle(15, 0, 15, 720, barrierColor);
            DrawRectangleLines(15, 0, 15, 720, (Color){ 255, 100, 120, 255 });
            DrawRectangleGradientH(30, 0, 30, 720, glowColor, Fade(glowColor, 0.0f));

            // Vách năng lượng bên phải (phạm vi đấu trường)
            float barrierRightX = (float)map->width * map->tilewidth - 30.0f;
            DrawRectangle((int)barrierRightX, 0, 15, 720, barrierColor);
            DrawRectangleLines((int)barrierRightX, 0, 15, 720, (Color){ 255, 100, 120, 255 });
            DrawRectangleGradientH((int)barrierRightX - 30, 0, 30, 720, Fade(glowColor, 0.0f), glowColor);
        }
    }

    // Vẽ người chơi
    DrawPlayer(&player, texIdle, texWalk, texRun, texJump, texAttack, texHurt, 64, 64, 1.0f);

    EndMode2D();
    EndTextureMode();

    // --- DRAW TO SCREEN ---
    BeginDrawing();
    ClearBackground(BLACK);
    
    Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height};
    Rectangle destRec = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    DrawTexturePro(target.texture, sourceRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
    
    // Giao diện người chơi (thanh máu dạng vạch màu hồng anh đào phát sáng)
    int startX = 35;
    int startY = 35;
    DrawRectangleRounded((Rectangle){ startX - 10, startY - 10, 200, 35 }, 0.25f, 4, (Color){ 20, 20, 25, 180 });
    DrawRectangleRoundedLines((Rectangle){ startX - 10, startY - 10, 200, 35 }, 0.25f, 4, 1.5f, (Color){ 100, 100, 110, 80 });
    DrawText("CAT HP", startX, startY - 2, 10, (Color){ 255, 105, 180, 255 });
    for (int i = 0; i < 9; i++) {
        Color color = (i < player.currentHP) ? (Color){ 255, 60, 100, 255 } : (Color){ 55, 55, 65, 180 };
        DrawRectangle(startX + 45 + i * 14, startY - 3, 10, 12, color);
        if (i < player.currentHP) {
            DrawRectangleLines(startX + 45 + i * 14, startY - 3, 10, 12, (Color){ 255, 192, 203, 180 });
        }
    }

    if (currentMapIndex == 1) {
        if (introState == INTRO_FIGHT && bossInitialized) {
            DrawBossHP(&boss);
        }
        
        // Giao diện giới thiệu Boss
        if (introState == INTRO_SHOW_NAME) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.45f));
            DrawRectangle(0, SCREEN_HEIGHT / 2 - 120, SCREEN_WIDTH, 240, Fade(MAROON, 0.7f));
            DrawRectangle(0, SCREEN_HEIGHT / 2 - 110, SCREEN_WIDTH, 220, Fade(BLACK, 0.85f));
            
            const char* warningStr = "WARNING";
            const char* nameStr = "THE STORM ARCHMAGE";
            const char* subStr = "Guardian of the Red Lightning";
            
            int wWidth = MeasureText(warningStr, 40);
            int nWidth = MeasureText(nameStr, 70);
            int sWidth = MeasureText(subStr, 30);
            
            if ((int)(introTimer * 8) % 2 == 0) {
                DrawText(warningStr, SCREEN_WIDTH / 2 - wWidth / 2, SCREEN_HEIGHT / 2 - 90, 40, RED);
            } else {
                DrawText(warningStr, SCREEN_WIDTH / 2 - wWidth / 2, SCREEN_HEIGHT / 2 - 90, 40, Fade(RED, 0.3f));
            }
            
            DrawText(nameStr, SCREEN_WIDTH / 2 - nWidth / 2 + 3, SCREEN_HEIGHT / 2 - 32, 70, MAROON);
            DrawText(nameStr, SCREEN_WIDTH / 2 - nWidth / 2, SCREEN_HEIGHT / 2 - 35, 70, ORANGE);
            DrawText(subStr, SCREEN_WIDTH / 2 - sWidth / 2, SCREEN_HEIGHT / 2 + 50, 30, GRAY);
        }

        // Giao diện Game Over khi hết máu
        if (player.currentHP <= 0.0f) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.75f));
            const char* gameOverText = "YOU DIED";
            const char* restartText = "Press [R] to Restart the Battle";
            int goWidth = MeasureText(gameOverText, 70);
            int rstWidth = MeasureText(restartText, 25);
            DrawText(gameOverText, SCREEN_WIDTH / 2 - goWidth / 2 + 2, SCREEN_HEIGHT / 2 - 50 + 2, 70, BLACK);
            DrawText(gameOverText, SCREEN_WIDTH / 2 - goWidth / 2, SCREEN_HEIGHT / 2 - 50, 70, (Color){ 220, 30, 45, 255 });
            DrawText(restartText, SCREEN_WIDTH / 2 - rstWidth / 2, SCREEN_HEIGHT / 2 + 40, 25, GRAY);
        }
    }
    
    DrawFPS(10, 10);
    EndDrawing();
  }

  UnloadTexture(texIdle);
  UnloadTexture(texWalk);
  UnloadTexture(texRun);
  UnloadTexture(texJump);
  UnloadTexture(texAttack);
  UnloadTexture(texHurt);
  UnloadTexture(texTiles);
  UnloadBossResources();
  UnloadRenderTexture(target);
  UnloadSound(introSound);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
