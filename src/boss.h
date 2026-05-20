#ifndef BOSS_H
#define BOSS_H

#include "raylib.h"
#include "game.h"
#include "camera.h"

typedef enum {
    BOSS_STATE_IDLE = 0,
    BOSS_STATE_WALK,
    BOSS_STATE_ATTACK,
    BOSS_STATE_CAST,
    BOSS_STATE_HURT,
    BOSS_STATE_DEATH
} BossState;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   radius;
    bool    active;
    float   damage;
    int     currentFrame;
    float   frameTimer;
    int     maxFrames;
} BossProjectile;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   maxHP;
    float   currentHP;
    bool    isAlive;
    bool    isAttacking;
    bool    isCasting;
    bool    isHurt;
    int     currentPhase;
    int     attackPattern;
    float   attackTimer;
    float   attackCooldown;
    
    // State and Animation
    BossState state;
    int     currentFrame;
    float   frameTimer;
    bool    facingRight;
    float   stateTimer;
    
    // Projectiles/Spells
    BossProjectile projectile;
} Boss;

// Lifecycle functions
void InitBoss(Boss *boss, Vector2 pos);
void LoadBossResources(void);
void UnloadBossResources(void);

// Cập nhật Boss
void UpdateBoss(Boss *boss, Player *player, MyCamera *cam, float dt);

// Vẽ Boss
void DrawBoss(Boss *boss, float scale);

// Vẽ thanh máu của Boss
void DrawBossHP(Boss *boss);


#endif // BOSS_H
