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

  // 1. Load Bản đồ ban đầu (tutorial map)
  cute_tiled_map_t* map = NULL;
  if (currentMapIndex == 0) {
      map = MapLoad(mapFiles[currentMapIndex]);
      if (!map) return -1;
  }

  InitParticles();

  // 2. Khởi tạo Nhân vật và Camera
  Vector2 spawnPoint = {0};
  if (currentMapIndex == 0 && map) {
      spawnPoint = MapGetSpawnPoint(map);
  } else {
      spawnPoint = (Vector2){-50.0f, 640.0f};
  }

  Player player = {0};
  InitPlayer(&player, spawnPoint);
  if (currentMapIndex == 0 && map) {
      player.groundY = MapGetGroundY(map, player.position.x, player.position.y);
      if (player.position.y < player.groundY) {
          player.isJumping = true;
      }
      player.controlsEnabled = true; // Bắt đầu ở tutorial map thì tự do di chuyển
  } else {
      player.groundY = 640.0f;
      player.controlsEnabled = false; // Bắt đầu ở màn Boss thì chạy intro
  }

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
  if (currentMapIndex == 0 && map) {
      float mapW = (float)map->width * map->tilewidth;
      float mapH = (float)map->height * map->tileheight;
      CameraSetBounds(&myCam, mapW, mapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
      float zoomX = (float)VIRTUAL_WIDTH / mapW;
      float zoomY = (float)VIRTUAL_HEIGHT / mapH;
      myCam.zoom = (zoomX > zoomY) ? zoomX : zoomY;
      myCam.zoom += 0.02f;
      if (myCam.zoom < 1.2f) myCam.zoom = 1.2f;
  } else {
      myCam.zoom = 1.0f;
      CameraSetBounds(&myCam, 2500.0f, 720.0f, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
  }

  Boss boss;
  bool bossInitialized = false;
  LoadBossResources();

  // --- CẤU HÌNH INTRO CHUYỂN CẢNH ---
  IntroState introState = (currentMapIndex == 1) ? INTRO_WALK_IN : INTRO_FIGHT;
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
            // Reset lại người chơi ở map Boss (spawn phía bên trái)
            player.position = (Vector2){-50.0f, 640.0f};
            player.groundY = 640.0f;
            player.velocity = (Vector2){0, 0};
            player.isJumping = false;
            player.currentHP = player.maxHP;
            player.hurtTimer = 0.0f;
            player.controlsEnabled = false;

            // Reset lại Boss bên phải
            InitBoss(&boss, (Vector2){950.0f, 640.0f});
            bossInitialized = true;

            // Reset lại Intro
            introState = INTRO_WALK_IN;
            introTimer = 0.0f;
            myCam.zoom = 1.0f;
            myCam.boundsEnabled = false;
            CameraLookAt(&myCam, (Vector2){player.position.x, player.position.y - 86.0f});
            printf("Restarted the Boss Fight in Placeholder Arena!\n");
        } else {
            // Reset ở map hướng dẫn
            if (map) {
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
    }

    // --- PHÍM TẮT CHUYỂN NHANH ĐẾN BOSS ARENA ('B') ---
    if (IsKeyPressed(KEY_B)) {
        if (currentMapIndex != 1) {
            currentMapIndex = 1;
            if (map) {
                MapUnload(map);
                map = NULL;
            }
            player.position = (Vector2){-50.0f, 640.0f};
            player.groundY = 640.0f;
            player.velocity = (Vector2){0, 0};
            player.isJumping = false;
            player.controlsEnabled = false; // Khóa phím khi chạy intro

            // Khởi tạo Boss ở bên phải
            InitBoss(&boss, (Vector2){950.0f, 640.0f});
            bossInitialized = true;

            myCam.zoom = 1.0f;
            myCam.boundsEnabled = false;
            CameraLookAt(&myCam, (Vector2){player.position.x, player.position.y - 86.0f});

            // Bắt đầu chuỗi Intro giới thiệu Boss
            introState = INTRO_WALK_IN;
            introTimer = 0.0f;
            printf("Teleported to original Boss Arena via [B]! Spawn X: -50.0f. Start Intro.\n");
        }
    }

    // --- CẬP NHẬT TRẠNG THÁI GAME ---
    if (currentMapIndex == 1) {
        UpdateParticles(dt);
    }

    // --- CẬP NHẬT THEO TỪNG MAP ---
    if (currentMapIndex == 0) {
        // Map 0: Tutorial Map
        if (map) {
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
                map = NULL;

                player.position = (Vector2){-50.0f, 640.0f};
                player.groundY = 640.0f;
                player.velocity = (Vector2){0, 0};
                player.isJumping = false;
                player.controlsEnabled = false; // Khóa phím khi chạy intro

                // Khởi tạo Boss ở bên phải
                InitBoss(&boss, (Vector2){950.0f, 640.0f});
                bossInitialized = true;

                myCam.zoom = 1.0f;
                myCam.boundsEnabled = false;
                CameraLookAt(&myCam, (Vector2){player.position.x, player.position.y - 86.0f});

                // Bắt đầu chuỗi Intro giới thiệu Boss
                introState = INTRO_WALK_IN;
                introTimer = 0.0f;
                printf("Transitioned to Boss Arena! Spawn X: -50.0f. Start Intro.\n");
            } else {
                // Giới hạn trong map
                float mapW = (float)map->width * map->tilewidth;
                if (player.position.x > mapW - 16.0f) player.position.x = mapW - 16.0f;
            }

            // Camera follow player
            Vector2 camTarget = { player.position.x, player.position.y - 86.0f };
            CameraUpdate(&myCam, camTarget, dt);
        }

    } else if (currentMapIndex == 1) {
        // Map 1: Boss Arena (Màn hình placeholder vẽ đá)
        player.groundY = 640.0f;

        // --- CẬP NHẬT INTRO & GAMEPLAY STATE ---
        if (introState == INTRO_WALK_IN) {
            // Mèo tự động di chuyển sang phải
            player.position.x += 160.0f * dt;
            player.facingRight = true;
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

            if (player.position.x >= 350.0f) {
                player.position.x = 350.0f;
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
                UpdatePlayer(&player, dt);

                if (player.controlsEnabled && player.currentHP > 0.0f) {
                    if (bossInitialized && boss.isAlive) {
                        // Khóa trong đấu trường bởi barriers
                        if (player.position.x < 35.0f) player.position.x = 35.0f;
                        if (player.position.x > 1315.0f) player.position.x = 1315.0f;
                    } else {
                        // Boss đã chết, cho phép đi qua rào chắn sang phải để chuyển map
                        if (player.position.x < 16.0f) player.position.x = 16.0f;
                        if (player.position.x > 1315.0f) {
                            // Chuyển ngược lại về tutorial map
                            currentMapIndex = 0;
                            map = MapLoad(mapFiles[currentMapIndex]);
                            if (map) {
                                float newMapW = (float)map->width * map->tilewidth;
                                float newMapH = (float)map->height * map->tileheight;
                                player.position.x = 24.0f;
                                player.position.y = MapGetGroundY(map, player.position.x, -9999.0f);
                                player.groundY = player.position.y;
                                player.velocity = (Vector2){0, 0};
                                player.isJumping = false;
                                player.controlsEnabled = true;

                                bossInitialized = false;

                                CameraSetBounds(&myCam, newMapW, newMapH, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                                float zoomX = (float)VIRTUAL_WIDTH / newMapW;
                                float zoomY = (float)VIRTUAL_HEIGHT / newMapH;
                                myCam.zoom = (zoomX > zoomY) ? zoomX : zoomY;
                                myCam.zoom += 0.02f;
                                if (myCam.zoom < 1.2f) myCam.zoom = 1.2f;

                                introState = INTRO_FIGHT;
                                printf("Escaped Boss Arena back to tutorial map! Spawn X: %.1f\n", player.position.x);
                            }
                        }
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

        // Cập nhật Camera Target
        Vector2 camTarget = { player.position.x, player.position.y - 86.0f };
        if (introState == INTRO_PAN_TO_BOSS && bossInitialized) {
            camTarget = (Vector2){ boss.position.x, boss.position.y - 86.0f };
        }
        
        // Thiết lập bounds dựa vào trạng thái Intro
        if (introState == INTRO_FIGHT) {
            CameraSetBounds(&myCam, 1351.0f, 720.0f, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
        } else {
            // Không áp dụng giới hạn biên camera trong quá trình Intro để camera đi theo mèo và lia sang boss
            myCam.boundsEnabled = false;
        }
        
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

    if (currentMapIndex == 0 && map) {
        // Vẽ bản đồ Tiled
        cute_tiled_layer_t* layer = map->layers;
        while (layer) {
            if (layer->visible) {
                MapDrawLayer(map, layer->name.ptr, 0.0f, texTiles);
            }
            layer = layer->next;
        }
    }

    if (currentMapIndex == 1) {
        // Đấu trường đá (Stone Platform Altar) tự vẽ
        DrawRectangleRec((Rectangle){ -1500, 640, 5000, 384 }, (Color){16, 16, 24, 255});
        
        // Vân kẻ ô lát đá
        for (float gx = -1500; gx < 3500; gx += 64) {
            DrawLineV((Vector2){ gx, 640 }, (Vector2){ gx, 1024 }, (Color){28, 28, 40, 255});
        }
        for (float gy = 640; gy < 1024; gy += 64) {
            DrawLineV((Vector2){ -1500, gy }, (Vector2){ 3500, gy }, (Color){28, 28, 40, 255});
        }
        
        // Viền Neon Đỏ phát sáng
        DrawLineEx((Vector2){ -1500, 640 }, (Vector2){ 3500, 640 }, 4.0f, (Color){255, 46, 99, 255});
        DrawLineEx((Vector2){ -1500, 639 }, (Vector2){ 3500, 639 }, 8.0f, Fade((Color){255, 46, 99, 255}, 0.25f));
        DrawLineEx((Vector2){ -1500, 638 }, (Vector2){ 3500, 638 }, 14.0f, Fade((Color){255, 46, 99, 255}, 0.1f));

        // Hiệu ứng hạt bụi phép vàng trôi lơ lửng
        DrawParticles();
        
        // Vẽ Boss
        if (bossInitialized) {
            DrawBoss(&boss, 2.0f);
        }

        // Vẽ vách ngăn ma thuật biên giới hạn khu vực đấu (Dead Cells style)
        if (introState == INTRO_FIGHT && bossInitialized && boss.isAlive) {
            float barrierAlpha = 0.35f + 0.15f * sinf(GetTime() * 7.0f);
            Color barrierColor = (Color){ 255, 46, 99, (unsigned char)(barrierAlpha * 255) };
            Color glowColor = (Color){ 255, 46, 99, 80 };
            
            // Vách năng lượng bên trái (x = 15 đến 30)
            DrawRectangle(15, 0, 15, 640, barrierColor);
            DrawRectangleLines(15, 0, 15, 640, (Color){ 255, 100, 120, 255 });
            DrawRectangleGradientH(30, 0, 30, 640, glowColor, Fade(glowColor, 0.0f));

            // Vách năng lượng bên phải (x = 1320 đến 1335)
            DrawRectangle(1320, 0, 15, 640, barrierColor);
            DrawRectangleLines(1320, 0, 15, 640, (Color){ 255, 100, 120, 255 });
            DrawRectangleGradientH(1290, 0, 30, 640, Fade(glowColor, 0.0f), glowColor);
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
  if (map) {
      MapUnload(map);
  }
  return 0;
}
