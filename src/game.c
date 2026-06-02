#include "game.h"
#include "raylib.h"
#include <math.h>

void InitPlayer(Player *player, Vector2 pos) {
    player->position = pos;
    player->velocity = (Vector2){0, 0};
    player->speed = (Vector2){300.0f, 0}; // Tốc độ di chuyển ngang mặc định
    player->groundY = pos.y;
    player->facingRight = true;
    player->isJumping = false;
    player->isAttacking = false;
    player->freezeTimer = 0.0f;
    player->currentFrame = 0;
    player->frameTimer = 0.0f;
    player->controlsEnabled = true;
    player->isHurt = false;
    player->maxHP = 9.0f;
    player->currentHP = 9.0f;
    player->hurtTimer = 0.0f;
}

void UpdatePlayer(Player *player, float deltaTime) {
    const float gravity = 4000.0f;
    const float jumpForce = -1200.0f;  // Tăng từ -900 lên -1200 để nhảy cao qua đầu Boss
    
    player->isRunning = false; 
    player->isSprinting = false;
    float currentSpeed = player->speed.x;
    float animSpeed = 0.1f; 

    if (player->freezeTimer > 0) player->freezeTimer -= deltaTime;
    if (player->hurtTimer > 0) player->hurtTimer -= deltaTime;

    if (player->controlsEnabled) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) { 
            currentSpeed *= 1.8f; 
            player->isSprinting = true; 
            animSpeed = 0.07f; 
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !player->isAttacking && !player->isHurt) {
            player->isAttacking = true;
            player->currentFrame = 0;
            player->frameTimer = 0.0f;
            player->freezeTimer = 0.15f;
        }

        if (player->isAttacking) animSpeed = 0.07f;
        if (player->isHurt) animSpeed = 0.12f;
        
        // Nhảy bằng SPACE hoặc phím W
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && !player->isJumping) {
            player->velocity.y = jumpForce;
            player->isJumping = true;
            if (!player->isAttacking) {
                player->currentFrame = 0;
                player->frameTimer = 0.0f;
            }
        }

        // Cho phép điều khiển ngang cả khi đang trên không bằng phím mũi tên hoặc A/D
        if (player->freezeTimer <= 0) {
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                player->position.x += currentSpeed * deltaTime;
                player->facingRight = true;
                player->isRunning = true;
            }
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                player->position.x -= currentSpeed * deltaTime;
                player->facingRight = false;
                player->isRunning = true;
            }
        }
    }

    if (player->position.y < player->groundY) {
        player->isJumping = true;
    }

    if (player->isJumping) {
        player->velocity.y += gravity * deltaTime;
        player->position.y += player->velocity.y * deltaTime;
        if (!player->isAttacking) animSpeed = 0.1f; 
    }

    if (player->position.y >= player->groundY) {
        player->position.y = player->groundY;
        player->velocity.y = 0;
        player->isJumping = false;
    }

    player->frameTimer += deltaTime;
    if (player->frameTimer >= animSpeed) {
        player->frameTimer = 0;
        player->currentFrame++;
    }

    if (player->isAttacking && player->currentFrame >= 8) {
        player->isAttacking = false;
    }
    
    if (player->isHurt && player->currentFrame >= 4) {
        player->isHurt = false;
    }

    // Clamp horizontal position when controls are enabled and player is alive
    if (player->controlsEnabled && player->currentHP > 0.0f) {
        if (player->position.x < 35.0f) player->position.x = 35.0f;
        if (player->position.x > 1315.0f) player->position.x = 1315.0f;
    }
}

void DrawPlayer(Player *player, Texture2D idle, Texture2D walk, Texture2D run, Texture2D jump, Texture2D attack, Texture2D hurt, int frameW, int frameH, float scale) {
    Texture2D currentTex = idle;
    int maxFrames = 10;
    int spacing = 16; // Spacing 16px
    int frameIdx = player->currentFrame % maxFrames;

    if (player->isHurt) { currentTex = hurt; maxFrames = 4; }
    else if (player->isAttacking) { currentTex = attack; maxFrames = 8; }
    else if (player->isJumping) { currentTex = jump; maxFrames = 3; }
    else {
        if (player->isRunning) {
            if (player->isSprinting) { currentTex = run; maxFrames = 8; }
            else { currentTex = walk; maxFrames = 12; }
        }
        frameIdx = player->currentFrame % maxFrames;
    }

    if (player->isHurt || player->isAttacking || player->isJumping) {
        frameIdx = player->currentFrame % maxFrames;
    }

    float frameX = (float)frameIdx * (frameW + spacing);
    float yOffset = 16.0f;
    Rectangle source = { frameX, 0, (float)frameW, (float)frameH };
    
    // Hướng nhìn chuẩn của Sprite mèo
    if (player->facingRight) source.width = -source.width;
    
    Rectangle dest = { roundf(player->position.x), roundf(player->position.y + yOffset), (float)frameW * scale, (float)frameH * scale };
    Color tint = WHITE;
    if (player->hurtTimer > 0.0f) {
        // Nhấp nháy màu đỏ khi bị thương
        if (((int)(player->hurtTimer * 15.0f)) % 2 == 0) {
            tint = RED;
        }
    }
    DrawTexturePro(currentTex, source, dest, (Vector2){(float)frameW * scale / 2.0f, (float)frameH * scale}, 0.0f, tint);
}
