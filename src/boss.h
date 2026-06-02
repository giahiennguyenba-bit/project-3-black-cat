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

typedef enum {
    SPELL_DARK_BOLT = 0,
    SPELL_FIRE_BOMB,
    SPELL_LIGHTNING,
    SPELL_DEATH_SPELL
} SpellType;

typedef struct {
    SpellType type;
    int     state;          // 0 = bay/cảnh báo, 1 = nổ/sét đánh
    float   stateTimer;
    Vector2 position;
    Vector2 velocity;
    Vector2 targetPos;
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
    BossProjectile extraProjectiles[2]; // Tia phép phụ phân mảnh từ FireBomb
    BossProjectile lightningStrikes[4]; // Đổi từ 3 thành 4 tia sét cùng lúc cho Thiên Lôi Phạt
    BossProjectile deathSpells[3];      // Ba cột phép tử thần (chỉ ở Phase 3)
    
    // Phase 2 Bullet Hell
    bool    auraActive;
    float   auraTimer;
    float   auraSparkTimer;
    BossProjectile auraSparks[4];       // 4 tia đạn bám đuổi của Ma Thuật Hộ Thân
    bool    hasCheckedBehind;           // Đánh dấu kiểm tra 30% né sau lưng
    bool    hasTriggeredPhase2Aura;     // Đã kích hoạt Aura Spark ở đầu Phase 2 chưa

    // Rage Mode (Phase 2: 3 đòn liên tiếp kích hoạt Rage)
    bool    rageActive;                 // Boss đang trong trạng thái Rage
    float   rageTimer;                  // Thời gian còn lại của Rage (5 giây)
    int     consecutiveHits;            // Đếm số đòn liên tiếp nhận từ Mèo
    float   hitResetTimer;              // Timer reset bộ đếm nếu ngừng bị đánh
    
    float   invincibilityTimer;         // I-frames sau khi trúng đòn
    float   forcefieldTimer;            // Vòng bảo vệ cản vật lý (Concierge style)
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
