#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

// --- Cấu trúc Nhân vật ---
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 speed;
    float   groundY;
    bool    facingRight;
    bool    isJumping;
    bool    isRunning;
    bool    isSprinting;
    bool    isAttacking;
    bool    isHurt;
    float   freezeTimer;
    int     currentFrame;
    float   frameTimer;
    bool    controlsEnabled;
    float   maxHP;
    float   currentHP;
    float   hurtTimer;
    float   jumpBufferTimer;
    float   coyoteTimeTimer;
} Player;

// --- API Nhân vật ---
void InitPlayer(Player *player, Vector2 pos);
void UpdatePlayer(Player *player, float deltaTime);
void DrawPlayer(Player *player, Texture2D idle, Texture2D walk, Texture2D run, Texture2D jump, Texture2D attack, Texture2D hurt, int frameW, int frameH, float scale);

#endif // GAME_H
