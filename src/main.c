#include "camera.h"
#include "game.h"
#include "map.h"
#include "boss.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define VIRTUAL_HEIGHT 760
#define VIRTUAL_WIDTH 1351

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

int main(void) {
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Forest - Boss Altar Arena");
  InitAudioDevice(); // Kích hoạt hệ thống âm thanh
  SetTargetFPS(60);

  RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
  SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

  // 1. Cài đặt mặt đất mặc định (không phụ thuộc vào file TMJ của Map Maker)
  float groundY = 640.0f;
  InitParticles();

  // 2. Khởi tạo Nhân vật và Camera
  Player player = {0};
  InitPlayer(&player, (Vector2){-50.0f, groundY}); // Bắt đầu ngoài rìa bên trái
  player.controlsEnabled = false; // Khóa điều khiển của người chơi lúc đầu

  Texture2D texIdle = LoadTexture("assets/cat_png/Cat-png/CAT-IDLE.png");
  Texture2D texWalk = LoadTexture("assets/cat_png/Cat-png/CAT-WALK.png");
  Texture2D texRun = LoadTexture("assets/cat_png/Cat-png/CAT-RUN.png");
  Texture2D texJump = LoadTexture("assets/cat_png/Cat-png/CAT-JUMP.png");
  Texture2D texAttack = LoadTexture("assets/cat_png/Cat-png/CAT-ATTACK.png");
  Texture2D texHurt = LoadTexture("assets/cat_png/Cat-png/CAT-HURT.png");

  MyCamera myCam = CameraNew(player.position.x, player.position.y, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
  myCam.zoom = 1.2f;
  myCam.rl.offset.y = VIRTUAL_HEIGHT * 0.72f; 
  CameraSetSmoothDamped(&myCam, 10.0f);
  
  // Thiết lập biên bản đồ tự chế (arena rộng 2500px, cao 720px để tránh lội gạch dưới đất)
  CameraSetBounds(&myCam, 2500.0f, 720.0f, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

  Boss boss;
  InitBoss(&boss, (Vector2){950.0f, groundY}); // Đặt Boss xa về bên phải màn hình

  // --- CẤU HÌNH INTRO CHUYỂN CẢNH ---
  typedef enum {
      INTRO_WALK_IN = 0,
      INTRO_PAN_TO_BOSS,
      INTRO_SHOW_NAME,
      INTRO_FIGHT
  } IntroState;
  IntroState introState = INTRO_WALK_IN;
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
        // Reset lại người chơi
        InitPlayer(&player, (Vector2){-50.0f, groundY});
        player.controlsEnabled = false;

        // Reset lại Boss
        InitBoss(&boss, (Vector2){950.0f, groundY});

        // Reset lại Intro
        introState = INTRO_WALK_IN;
        introTimer = 0.0f;
        
        printf("Restarted the Boss Fight!\n");
    }

    // --- CẬP NHẬT TRẠNG THÁI GAME ---
    UpdateParticles(dt);

    // --- CẬP NHẬT INTRO & GAMEPLAY STATE ---
    if (introState == INTRO_WALK_IN) {
        // Mèo tự động di chuyển sang phải
        player.position.x += 160.0f * dt; // Tốc độ đi bộ của mèo
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

        // Cập nhật Boss đứng yên thở (Idle animation)
        boss.state = BOSS_STATE_IDLE;
        boss.frameTimer += dt;
        if (boss.frameTimer >= 0.12f) {
            boss.frameTimer = 0.0f;
            boss.currentFrame = (boss.currentFrame + 1) % 8;
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
        UpdatePlayer(&player, dt); // Stand idle

        // Cập nhật Boss đứng yên thở (Idle animation)
        boss.state = BOSS_STATE_IDLE;
        boss.frameTimer += dt;
        if (boss.frameTimer >= 0.12f) {
            boss.frameTimer = 0.0f;
            boss.currentFrame = (boss.currentFrame + 1) % 8;
        }
    }
    else if (introState == INTRO_SHOW_NAME) {
        introTimer += dt;
        if (introTimer >= 2.8f) {
            introState = INTRO_FIGHT;
            player.controlsEnabled = true; // Trả lại điều khiển cho người chơi
        }

        UpdatePlayer(&player, dt); // Stand idle

        // Cập nhật Boss đứng yên thở (Idle animation)
        boss.state = BOSS_STATE_IDLE;
        boss.frameTimer += dt;
        if (boss.frameTimer >= 0.12f) {
            boss.frameTimer = 0.0f;
            boss.currentFrame = (boss.currentFrame + 1) % 8;
        }
    }
    else {
        // Chế độ chơi chính thức
        if (player.currentHP > 0.0f) {
            UpdatePlayer(&player, dt);
            UpdateBoss(&boss, &player, &myCam, dt);
        } else {
            // Player chết -> khoá phím và đưa boss về trạng thái IDLE thở bình yên
            player.controlsEnabled = false;
            boss.state = BOSS_STATE_IDLE;
            boss.velocity.x = 0;
            boss.frameTimer += dt;
            if (boss.frameTimer >= 0.12f) {
                boss.frameTimer = 0.0f;
                boss.currentFrame = (boss.currentFrame + 1) % 8;
            }
        }
    }

    // Camera luôn follow player ở mọi trạng thái
    Vector2 camTarget = (Vector2){ player.position.x, player.position.y - 86.0f };

    if (introState == INTRO_FIGHT) {
        CameraSetBounds(&myCam, 1351.0f, 720.0f, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    } else {
        CameraSetBounds(&myCam, 2500.0f, 720.0f, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    }
    CameraUpdate(&myCam, camTarget, dt);

    // --- RENDER GAMEPLAY ---
    BeginTextureMode(target);
    ClearBackground(BLACK);

    // --- 1. RENDER BACKGROUND SKYBOX (Screen Space - không bị dịch chuyển bởi Camera) ---
    // Gradient nền trời đêm chuyển từ Xanh Navy sẫm sang Tím Hoàng Gia
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

    // --- 2. RENDER WORLD (World Space - dịch chuyển theo Camera) ---
    BeginMode2D(myCam.rl);

    // Vẽ phế tích cột đá cổ kính xa xăm (Hiệu ứng Parallax di chuyển chậm)
    float pX = myCam.rl.target.x * 0.3f;
    Color ruinsColor = (Color){22, 20, 36, 255};
    Color ruinsTrim = (Color){32, 28, 50, 255};
    for (int i = -5; i < 15; i++) {
        float x = i * 300.0f - pX;
        // Cột đá gothic chính
        DrawRectangleRec((Rectangle){ x, 250, 40, 390 }, ruinsColor);
        DrawRectangleRec((Rectangle){ x - 5, 240, 50, 10 }, ruinsTrim);
        DrawRectangleRec((Rectangle){ x - 2, 630, 44, 10 }, ruinsTrim);
        // Cột phụ gãy vụn tạo khí chất hoang tàn
        if (i % 2 == 0) {
            DrawRectangleRec((Rectangle){ x - 80, 400, 30, 240 }, ruinsColor);
            DrawRectangleRec((Rectangle){ x - 82, 390, 34, 10 }, ruinsTrim);
        }
    }

    // Đấu trường đá (Stone Platform Altar) tự vẽ
    float arenaWidth = 2500.0f;
    DrawRectangleRec((Rectangle){ -500, 640, arenaWidth + 1000, 384 }, (Color){16, 16, 24, 255});
    
    // Vân kẻ ô lát đá
    for (float gx = -500; gx < arenaWidth + 500; gx += 64) {
        DrawLineV((Vector2){ gx, 640 }, (Vector2){ gx, 1024 }, (Color){28, 28, 40, 255});
    }
    for (float gy = 640; gy < 1024; gy += 64) {
        DrawLineV((Vector2){ -500, gy }, (Vector2){ arenaWidth + 500, gy }, (Color){28, 28, 40, 255});
    }
    
    // Viền Neon Đỏ phát sáng báo hiệu ranh giới tử thần nguy hiểm của đấu trường
    DrawLineEx((Vector2){ -500, 640 }, (Vector2){ arenaWidth + 500, 640 }, 4.0f, (Color){255, 46, 99, 255});
    DrawLineEx((Vector2){ -500, 639 }, (Vector2){ arenaWidth + 500, 639 }, 8.0f, Fade((Color){255, 46, 99, 255}, 0.25f));
    DrawLineEx((Vector2){ -500, 638 }, (Vector2){ arenaWidth + 500, 638 }, 14.0f, Fade((Color){255, 46, 99, 255}, 0.1f));

    // Hiệu ứng hạt bụi phép vàng trôi lơ lửng
    DrawParticles();

    // Vẽ nhân vật và Boss
    DrawPlayer(&player, texIdle, texWalk, texRun, texJump, texAttack, texHurt, 64, 64, 1.0f);
    DrawBoss(&boss, 2.0f);

    // Vẽ vách ngăn ma thuật biên giới hạn khu vực đấu (Dead Cells style)
    if (introState == INTRO_FIGHT) {
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

    EndMode2D();
    EndTextureMode();

    // --- DRAW TO SCREEN ---
    BeginDrawing();
    ClearBackground(BLACK);
    
    Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height};
    Rectangle destRec = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    DrawTexturePro(target.texture, sourceRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
    
    if (introState == INTRO_FIGHT) {
        DrawBossHP(&boss);
        DrawFPS(10, 10);
        
        // Vẽ thanh máu của Mèo (9 HP) dạng các vạch năng lượng màu hồng anh đào phát sáng
        int startX = 35;
        int startY = 35;
        // Khung nền mờ cao cấp cho máu Mèo (Glassmorphism)
        DrawRectangleRounded((Rectangle){ startX - 10, startY - 10, 200, 35 }, 0.25f, 4, (Color){ 20, 20, 25, 180 });
        DrawRectangleRoundedLines((Rectangle){ startX - 10, startY - 10, 200, 35 }, 0.25f, 4, 1.5f, (Color){ 100, 100, 110, 80 });

        DrawText("CAT HP", startX, startY - 2, 10, (Color){ 255, 105, 180, 255 });
        for (int i = 0; i < 9; i++) {
            Color color = (i < player.currentHP) ? (Color){ 255, 60, 100, 255 } : (Color){ 55, 55, 65, 180 };
            DrawRectangle(startX + 45 + i * 14, startY - 3, 10, 12, color);
            if (i < player.currentHP) {
                // Hiệu ứng viền phát sáng nhẹ
                DrawRectangleLines(startX + 45 + i * 14, startY - 3, 10, 12, (Color){ 255, 192, 203, 180 });
            }
        }
    }

    // Giao diện Game Over khi Mèo hết máu
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
    
    // Giao diện giới thiệu Boss hoành tráng (Chỉ hiện khi INTRO_SHOW_NAME)
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
    
    EndDrawing();
  }

  UnloadTexture(texIdle);
  UnloadTexture(texWalk);
  UnloadTexture(texRun);
  UnloadTexture(texJump);
  UnloadTexture(texAttack);
  UnloadTexture(texHurt);
  UnloadBossResources();
  UnloadRenderTexture(target);
  UnloadSound(introSound); // Giải phóng tiếng sấm intro
  CloseAudioDevice(); // Đóng âm thanh
  CloseWindow();
  return 0;
}
