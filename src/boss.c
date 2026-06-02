#include "boss.h"
#include "camera.h"
#include "raymath.h"
#include <math.h>

// Mảng texture lưu trữ các khung hình hoạt ảnh của Boss
static Texture2D texBossIdle[8];
static Texture2D texBossWalk[8];
static Texture2D texBossAttack[10];
static Texture2D texBossCast[9];
static Texture2D texBossHurt[3];
static Texture2D texBossDeath[10]; // Bringer-of-Death_Death_2.png to _10.png (9 tiles in TSX but let's load all 10)

// Mảng texture lưu trữ các khung hình của Spell
static Texture2D texSpellDarkBolt[12];
static Texture2D texSpellFireBomb[15];
static Texture2D texSpellLightning[11];
static Texture2D texSpellDeathSpell[16];
static Texture2D texSpellSpark[7];

// Trạng thái tài nguyên đã tải chưa
static bool resourcesLoaded = false;

void LoadBossResources(void) {
    if (resourcesLoaded) return;

    // Tải ảnh IDLE (8 frames)
    for (int i = 0; i < 8; i++) {
        texBossIdle[i] = LoadTexture(TextFormat("assets/boss_png/BOSS-PNG/BOSS-IDLE/Bringer-of-Death_Idle_%d.png", i + 1));
    }

    // Tải ảnh WALK (8 frames)
    for (int i = 0; i < 8; i++) {
        texBossWalk[i] = LoadTexture(TextFormat("assets/boss_png/BOSS-PNG/BOSS-WALK/Bringer-of-Death_Walk_%d.png", i + 1));
    }

    // Tải ảnh ATTACK (10 frames)
    for (int i = 0; i < 10; i++) {
        texBossAttack[i] = LoadTexture(TextFormat("assets/boss_png/BOSS-PNG/BOSS-ATTACK/Bringer-of-Death_Attack_%d.png", i + 1));
    }

    // Tải ảnh CAST (9 frames)
    for (int i = 0; i < 9; i++) {
        texBossCast[i] = LoadTexture(TextFormat("assets/boss_png/BOSS-PNG/BOSS-CAST/Bringer-of-Death_Cast_%d.png", i + 1));
    }

    // Tải ảnh HURT (3 frames)
    for (int i = 0; i < 3; i++) {
        texBossHurt[i] = LoadTexture(TextFormat("assets/boss_png/BOSS-PNG/BOSS-HURT/Bringer-of-Death_Hurt_%d.png", i + 1));
    }

    // Tải ảnh DEATH (10 frames)
    for (int i = 0; i < 10; i++) {
        texBossDeath[i] = LoadTexture(TextFormat("assets/boss_png/BOSS-PNG/BOSS-DEATH/Bringer-of-Death_Death_%d.png", i + 1));
    }

    // Tải Spell DarkBolt (12 frames)
    for (int i = 0; i < 12; i++) {
        texSpellDarkBolt[i] = LoadTexture(TextFormat("assets/spell_png/SSPELL-PNG/DarkBolt/Dark-Bolt%d.png", i + 1));
    }

    // Tải Spell FireBomb (15 frames)
    for (int i = 0; i < 15; i++) {
        texSpellFireBomb[i] = LoadTexture(TextFormat("assets/spell_png/SSPELL-PNG/FireBomb/Fire-bomb%d.png", i + 1));
    }

    // Tải Spell Lightning (11 frames)
    for (int i = 0; i < 11; i++) {
        texSpellLightning[i] = LoadTexture(TextFormat("assets/spell_png/SSPELL-PNG/Lightning/Lightning%d.png", i + 1));
    }

    // Tải Spell DeathSpell (16 frames)
    for (int i = 0; i < 16; i++) {
        texSpellDeathSpell[i] = LoadTexture(TextFormat("assets/spell_png/SSPELL-PNG/SPELL/Bringer-of-Death_Spell_%d.png", i + 1));
    }

    // Tải Spell Spark (7 frames)
    for (int i = 0; i < 7; i++) {
        texSpellSpark[i] = LoadTexture(TextFormat("assets/spell_png/SSPELL-PNG/spark/spark%d.png", i + 1));
    }

    resourcesLoaded = true;
}

void UnloadBossResources(void) {
    if (!resourcesLoaded) return;

    for (int i = 0; i < 8; i++) {
        UnloadTexture(texBossIdle[i]);
        UnloadTexture(texBossWalk[i]);
    }
    for (int i = 0; i < 10; i++) {
        UnloadTexture(texBossAttack[i]);
        UnloadTexture(texBossDeath[i]);
    }
    for (int i = 0; i < 9; i++) {
        UnloadTexture(texBossCast[i]);
    }
    for (int i = 0; i < 3; i++) {
        UnloadTexture(texBossHurt[i]);
    }
    for (int i = 0; i < 12; i++) {
        UnloadTexture(texSpellDarkBolt[i]);
    }
    for (int i = 0; i < 15; i++) {
        UnloadTexture(texSpellFireBomb[i]);
    }
    for (int i = 0; i < 11; i++) {
        UnloadTexture(texSpellLightning[i]);
    }
    for (int i = 0; i < 16; i++) {
        UnloadTexture(texSpellDeathSpell[i]);
    }
    for (int i = 0; i < 7; i++) {
        UnloadTexture(texSpellSpark[i]);
    }

    resourcesLoaded = false;
}
void InitBoss(Boss *boss, Vector2 pos) {
    boss->position = pos;
    boss->velocity = (Vector2){0, 0};
    boss->maxHP = 100.0f;
    boss->currentHP = 100.0f;
    boss->isAlive = true;
    boss->isAttacking = false;
    boss->isCasting = false;
    boss->isHurt = false;
    boss->currentPhase = 1;
    boss->attackPattern = 0;
    boss->attackTimer = 0.0f;
    boss->attackCooldown = 2.0f; // 2 giây mỗi đòn
    
    boss->state = BOSS_STATE_IDLE;
    boss->currentFrame = 0;
    boss->frameTimer = 0.0f;
    boss->facingRight = false; // Mặc định quay về bên trái (hướng player)
    boss->stateTimer = 0.0f;
    
    // Khởi tạo projectile
    boss->projectile.active = false;
    boss->projectile.radius = 20.0f;
    boss->projectile.damage = 15.0f;
    boss->projectile.currentFrame = 0;
    boss->projectile.frameTimer = 0.0f;
    boss->projectile.maxFrames = 12;
    for (int i = 0; i < 2; i++) {
        boss->extraProjectiles[i].active = false;
    }
    for (int i = 0; i < 4; i++) {
        boss->lightningStrikes[i].active = false;
    }
    for (int i = 0; i < 3; i++) {
        boss->deathSpells[i].active = false;
    }    
    
    // Khởi tạo các biến Phase 2 Bullet Hell
    boss->auraActive = false;
    boss->auraTimer = 0.0f;
    boss->auraSparkTimer = 0.0f;
    boss->hasCheckedBehind = false;
    boss->hasTriggeredPhase2Aura = false;
    for (int i = 0; i < 4; i++) {
        boss->auraSparks[i].active = false;
    }

    // Khởi tạo Rage Mode
    boss->rageActive = false;
    boss->rageTimer = 0.0f;
    boss->consecutiveHits = 0;
    boss->hitResetTimer = 0.0f;
    boss->invincibilityTimer = 0.0f;
    boss->forcefieldTimer = 0.0f;

    // Tự động load resources nếu chưa có
    LoadBossResources();
}

void UpdateBoss(Boss *boss, Player *player, MyCamera *cam, float dt) {
    if (!boss->isAlive) return;
    // DEBUG: Nhấn phím T để giảm máu boss xuống dưới 50, lập tức chuyển sang Phase 2
    if (IsKeyPressed(KEY_T)) {
        boss->currentHP = 49.0f;
    }

    // Xác định Phase và tự động kích hoạt Ma Thuật Hộ Thân (Chiêu 3) duy nhất một lần ở đầu Phase 2
    if (boss->currentHP >= 50.0f) {
        boss->currentPhase = 1;
    } else {
        boss->currentPhase = 2;
        if (!boss->hasTriggeredPhase2Aura) {
            boss->hasTriggeredPhase2Aura = true;
            boss->auraActive = true;
            boss->auraTimer = 5.0f; // Thời gian tồn tại của Aura là 5 giây
            boss->auraSparkTimer = 0.0f;
            TraceLog(LOG_INFO, "Boss triggered Aura Sparks once at the start of Phase 2!");
        }
    }
    // Cập nhật Rage Mode
    if (boss->rageActive) {
        boss->rageTimer -= dt;
        if (boss->rageTimer <= 0.0f) {
            boss->rageActive = false;
            boss->rageTimer = 0.0f;
            TraceLog(LOG_INFO, "Boss Rage Mode ended!");
        }
    }
    // Cập nhật timer reset consecutive hits
    if (boss->hitResetTimer > 0.0f) {
        boss->hitResetTimer -= dt;
        if (boss->hitResetTimer <= 0.0f) {
            boss->consecutiveHits = 0; // Reset nếu ngừng bị đánh quá lâu
        }
    }
    // Cập nhật timer bất tử
    if (boss->invincibilityTimer > 0.0f) {
        boss->invincibilityTimer -= dt;
    }
    // Cập nhật timer vòng bảo vệ vật lý (Concierge style - Elliptical)
    if (boss->forcefieldTimer > 0.0f) {
        boss->forcefieldTimer -= dt;
        
        // Vật lý cản người chơi: Mèo không thể đi vào vòng lực lượng hình e-líp (bé lại và đẹp hơn)
        float rH = 75.0f; // Bán kính ngang e-líp
        float rV = 85.0f; // Bán kính dọc e-líp
        Vector2 bossCenter = { boss->position.x, boss->position.y - 45.0f };
        Vector2 playerCenter = { player->position.x, player->position.y - 32.0f };
        
        float dy = playerCenter.y - bossCenter.y;
        float yRatio = dy / rV;
        float yRatioSq = yRatio * yRatio;
        
        if (yRatioSq < 1.0f) {
            float dx = playerCenter.x - bossCenter.x;
            float expectedDx = rH * sqrtf(1.0f - yRatioSq);
            float absDx = fabsf(dx);
            if (absDx < expectedDx) {
                float pushDir = (dx >= 0.0f) ? 1.0f : -1.0f;
                // Đẩy lùi X của Mèo ra ngoài phạm vi hình e-líp bảo vệ
                player->position.x = boss->position.x + pushDir * (expectedDx + 5.0f);
                
                // Tạo vận tốc đẩy nhẹ ngược lại nếu mèo cố đi vào
                player->velocity.x = pushDir * 100.0f;
            }
        }
    }

    // Cập nhật chiêu đẩy gió mượt mà (Wind Push) trượt player ra xa từ từ
    if (boss->state == BOSS_STATE_CAST && boss->attackPattern == 99 && boss->currentFrame >= 5) {
        float pushDir = (player->position.x > boss->position.x) ? 1.0f : -1.0f;
        float targetDist = 360.0f; // Giảm 20% từ 450px xuống còn 360px
        float currentDist = fabsf(player->position.x - boss->position.x);
        if (currentDist < targetDist) {
            float pushSpeed = 1200.0f; // Trượt mèo với tốc độ 1200px/s
            player->position.x += pushDir * pushSpeed * dt;
            
            // Giới hạn khoảng cách không vượt quá targetDist
            float newDist = fabsf(player->position.x - boss->position.x);
            if (newDist > targetDist) {
                player->position.x = boss->position.x + pushDir * targetDist;
            }
            
            // Giới hạn trong biên đấu trường
            if (player->position.x < 35.0f) player->position.x = 35.0f;
            if (player->position.x > 1315.0f) player->position.x = 1315.0f;
        }
    }
    // Điều chỉnh attackCooldown dựa trên Phase và Rage
    // Phase 1: 1.3s (Rage: 0.65s) | Phase 2: 0.875s (Rage: 0.43s)
    if (boss->currentPhase == 1) {
        boss->attackCooldown = boss->rageActive ? 0.65f : 1.3f;
    } else {
        boss->attackCooldown = boss->rageActive ? 0.43f : 0.875f; // Rage: cực kỳ dồn dập
    }
    // Cập nhật trạng thái HURT / DEATH
    if (boss->currentHP <= 0 && boss->state != BOSS_STATE_DEATH) {
        boss->state = BOSS_STATE_DEATH;
        boss->currentFrame = 0;
        boss->frameTimer = 0.0f;
        boss->auraActive = false;
        for (int i = 0; i < 4; i++) {
            boss->auraSparks[i].active = false;
        }
    }

    // 1. Cập nhật hoạt ảnh và thời gian của các trạng thái đặc biệt
    boss->frameTimer += dt;
    float animSpeed = 0.12f;
    int maxFrames = 8;

    switch (boss->state) {
        case BOSS_STATE_IDLE:
            maxFrames = 8;
            if (boss->frameTimer >= animSpeed) {
                boss->frameTimer = 0;
                boss->currentFrame = (boss->currentFrame + 1) % maxFrames;
            }
            break;
            
        case BOSS_STATE_WALK:
            maxFrames = 8;
            if (boss->frameTimer >= animSpeed) {
                boss->frameTimer = 0;
                boss->currentFrame = (boss->currentFrame + 1) % maxFrames;
            }
            break;
        case BOSS_STATE_ATTACK:
            maxFrames = 10;
            animSpeed = 0.08f; // Chém nhanh
            if (boss->frameTimer >= animSpeed) {
                boss->frameTimer = 0;
                boss->currentFrame++;
                if (boss->currentFrame >= maxFrames) {
                    boss->state = BOSS_STATE_IDLE;
                    boss->currentFrame = 0;
                    boss->isAttacking = false;
                }
            }
            // Điểm chém trúng: Frame 5-7 (Hắc Ám Hoành Tảo)
            if (boss->currentFrame >= 5 && boss->currentFrame <= 7) {
                float dist = fabsf(boss->position.x - player->position.x);
                float heightDiff = fabsf(boss->position.y - player->position.y);
                
                // Chỉ trúng đòn khi đứng trước mặt Boss (Directional Hitbox)
                bool inFront = (boss->facingRight && player->position.x >= boss->position.x) || 
                               (!boss->facingRight && player->position.x <= boss->position.x);
                
                if (dist < 160.0f && heightDiff < 60.0f && inFront) {
                    // Người chơi bị dính đòn cận chiến!
                    if (player->freezeTimer <= 0) {
                        player->freezeTimer = 0.45f;
                        player->currentHP -= 1.0f;
                            player->isHurt = true;
                            player->currentFrame = 0;
                            player->frameTimer = 0.0f;
                            player->freezeTimer = 0.2f; // Mất 1 máu
                        // Đẩy mạnh player ra sau (120px) để tạo cảm giác chấn động mạnh
                        float pushDir = (player->position.x > boss->position.x) ? 1.0f : -1.0f;
                        player->position.x += pushDir * 120.0f;
                        CameraShake(cam, 0.35f, 10.0f); // Rung lắc màn hình mạnh hơn
                        TraceLog(LOG_INFO, "Player hit by Boss Melee (Hac Am Hoanh Tao)! Player HP: %.1f", player->currentHP);
                    }
                }
            }
            break;
            
        case BOSS_STATE_CAST:
            maxFrames = 9;
            animSpeed = 0.1f;
            if (boss->frameTimer >= animSpeed) {
                boss->frameTimer = 0;
                boss->currentFrame++;
                
                // Ở frame thứ 5, bắt đầu kích hoạt chiêu thức phép tương ứng
                if (boss->currentFrame == 5) {
                    if (boss->attackPattern == 99) {
                        // Wind Push: đẩy mèo ra xa chống cheese (độ xa giảm 20% còn 360, bắt đầu trượt mượt mà)
                        player->freezeTimer = 0.4f;
                        player->velocity.y = -300.0f; // nảy nhẹ lên
                        CameraShake(cam, 0.4f, 8.0f);
                        TraceLog(LOG_INFO, "Wind Push activated! Smooth push triggered.");
                        
                        // Ở Phase 2: Kích hoạt Aura Invincibility và Cản Vật Lý trong 6.2s
                        if (boss->currentPhase == 2) {
                            boss->invincibilityTimer = 6.2f;
                            boss->forcefieldTimer = 6.2f;
                            TraceLog(LOG_INFO, "Boss activated physical Concierge Forcefield in Phase 2 for 6.2s!");
                        }
                    }
                    else if (!boss->projectile.active) {
                        boss->projectile.active = true;
                        boss->projectile.type = (SpellType)boss->attackPattern;
                        boss->projectile.state = 0;
                        boss->projectile.stateTimer = 0.0f;
                        boss->projectile.currentFrame = 0;
                        boss->projectile.frameTimer = 0.0f;
                        
                        float dir = boss->facingRight ? 1.0f : -1.0f;
                        
                        if (boss->projectile.type == SPELL_DARK_BOLT) {
                            boss->projectile.position = (Vector2){ boss->position.x + (boss->facingRight ? 50.0f : -50.0f), boss->position.y - 45.0f };
                            boss->projectile.velocity = (Vector2){ dir * 700.0f, 0.0f }; // Tăng tốc độ bay từ 450 lên 700
                            boss->projectile.radius = 20.0f;
                            boss->projectile.damage = 15.0f;
                            boss->projectile.maxFrames = 12;
                        }
                        else if (boss->projectile.type == SPELL_FIRE_BOMB) {
                            // Cast trực tiếp dưới chân người chơi trên mặt đất (groundY)
                            boss->projectile.position = (Vector2){ player->position.x, player->groundY };
                            boss->projectile.velocity = (Vector2){ 0.0f, 0.0f };
                            boss->projectile.targetPos = player->position;
                            // Tăng bán kính 15% ở Phase 1 (50.0f * 1.15 = 57.5f) và 20% ở Phase 2 (50.0f * 1.2 = 60.0f)
                            boss->projectile.radius = (boss->currentPhase == 2) ? 60.0f : 57.5f;
                            boss->projectile.damage = 25.0f;
                            boss->projectile.maxFrames = 15; // 15 frames của Fire-bomb
                            boss->projectile.state = 1;      // Bỏ qua state 0 (ma thuật tím cảnh báo), vào thẳng bùng nổ
                        }
                        else if (boss->projectile.type == SPELL_LIGHTNING) {
                            // Kích hoạt 4 tia sét giáng xuống đồng thời làm Thiên Lôi Phạt!
                            // Bố trí quanh vị trí người chơi hiện tại: -240px, -80px, +80px, +240px
                            float offsets[4] = { -240.0f, -80.0f, 80.0f, 240.0f };
                            for (int i = 0; i < 4; i++) {
                                float randomOffset = (float)GetRandomValue(-15, 15);
                                boss->lightningStrikes[i].active = true;
                                boss->lightningStrikes[i].type = SPELL_LIGHTNING;
                                boss->lightningStrikes[i].state = 0; // Cảnh báo Telegraph
                                boss->lightningStrikes[i].stateTimer = 0.0f;
                                boss->lightningStrikes[i].position = (Vector2){ player->position.x + offsets[i] + randomOffset, player->groundY };
                                boss->lightningStrikes[i].radius = 30.0f; // Bức tường sét rộng 60px
                                boss->lightningStrikes[i].damage = 20.0f;
                                boss->lightningStrikes[i].maxFrames = 11;
                                boss->lightningStrikes[i].currentFrame = 0;
                                boss->lightningStrikes[i].frameTimer = 0.0f;
                            }
                            // Vô hiệu hóa đạn chính để tránh trùng lặp
                            boss->projectile.active = false;
                        }
                        else if (boss->projectile.type == SPELL_DEATH_SPELL) {
                            // X theo vị trí Player, Y dưới chân mèo
                            boss->projectile.position = (Vector2){ player->position.x, player->groundY };
                            boss->projectile.velocity = (Vector2){ 0, 0 };
                            boss->projectile.radius = 45.0f;
                            boss->projectile.damage = 35.0f;
                            boss->projectile.maxFrames = 16;
                            
                            if (boss->currentPhase == 3) {
                                // Kích hoạt thêm 2 cột phép phụ hai bên
                                for (int i = 0; i < 2; i++) {
                                    boss->deathSpells[i].active = true;
                                    boss->deathSpells[i].type = SPELL_DEATH_SPELL;
                                    boss->deathSpells[i].state = 0;
                                    boss->deathSpells[i].stateTimer = 0.0f;
                                    float offset = (i == 0) ? -120.0f : 120.0f;
                                    boss->deathSpells[i].position = (Vector2){ player->position.x + offset, player->groundY }; // Đặt dưới chân giống cột chính
                                    boss->deathSpells[i].velocity = (Vector2){ 0, 0 };
                                    boss->deathSpells[i].radius = 40.0f;
                                    boss->deathSpells[i].damage = 25.0f;
                                    boss->deathSpells[i].maxFrames = 16;
                                    boss->deathSpells[i].currentFrame = 0;
                                    boss->deathSpells[i].frameTimer = 0.0f;
                                }
                            }
                        }
                    }
                }
                
                // Sau 0.4s (4 frames từ frame 5), tức là ở frame 9, tung phép ngẫu nhiên cho chiêu đẩy gió
                if (boss->currentFrame == 9 && boss->attackPattern == 99) {
                    int randomSpell = GetRandomValue(0, 3);
                    boss->projectile.active = true;
                    boss->projectile.type = (SpellType)randomSpell;
                    boss->projectile.state = 0;
                    boss->projectile.stateTimer = 0.0f;
                    boss->projectile.currentFrame = 0;
                    boss->projectile.frameTimer = 0.0f;
                    
                    float dir = boss->facingRight ? 1.0f : -1.0f;
                    
                    if (boss->projectile.type == SPELL_DARK_BOLT) {
                        boss->projectile.position = (Vector2){ boss->position.x + (boss->facingRight ? 50.0f : -50.0f), boss->position.y - 45.0f };
                        boss->projectile.velocity = (Vector2){ dir * 700.0f, 0.0f };
                        boss->projectile.radius = 20.0f;
                        boss->projectile.damage = 15.0f;
                        boss->projectile.maxFrames = 12;
                    }
                    else if (boss->projectile.type == SPELL_FIRE_BOMB) {
                        boss->projectile.position = (Vector2){ player->position.x, player->groundY };
                        boss->projectile.velocity = (Vector2){ 0.0f, 0.0f };
                        boss->projectile.targetPos = player->position;
                        boss->projectile.radius = (boss->currentPhase == 2) ? 60.0f : 57.5f;
                        boss->projectile.damage = 25.0f;
                        boss->projectile.maxFrames = 15;
                        boss->projectile.state = 1; // Nổ ngay lập tức
                    }
                    else if (boss->projectile.type == SPELL_LIGHTNING) {
                        float offsets[4] = { -240.0f, -80.0f, 80.0f, 240.0f };
                        for (int j = 0; j < 4; j++) {
                            float randomOffset = (float)GetRandomValue(-15, 15);
                            boss->lightningStrikes[j].active = true;
                            boss->lightningStrikes[j].type = SPELL_LIGHTNING;
                            boss->lightningStrikes[j].state = 0;
                            boss->lightningStrikes[j].stateTimer = 0.0f;
                            boss->lightningStrikes[j].position = (Vector2){ player->position.x + offsets[j] + randomOffset, player->groundY };
                            boss->lightningStrikes[j].radius = 30.0f;
                            boss->lightningStrikes[j].damage = 20.0f;
                            boss->lightningStrikes[j].maxFrames = 11;
                            boss->lightningStrikes[j].currentFrame = 0;
                            boss->lightningStrikes[j].frameTimer = 0.0f;
                        }
                        boss->projectile.active = false;
                    }
                    else if (boss->projectile.type == SPELL_DEATH_SPELL) {
                        boss->projectile.position = (Vector2){ player->position.x, player->groundY };
                        boss->projectile.velocity = (Vector2){ 0, 0 };
                        boss->projectile.radius = 45.0f;
                        boss->projectile.damage = 35.0f;
                        boss->projectile.maxFrames = 16;
                        
                        if (boss->currentPhase == 3) {
                            for (int j = 0; j < 2; j++) {
                                boss->deathSpells[j].active = true;
                                boss->deathSpells[j].type = SPELL_DEATH_SPELL;
                                boss->deathSpells[j].state = 0;
                                boss->deathSpells[j].stateTimer = 0.0f;
                                float offset = (j == 0) ? -120.0f : 120.0f;
                                boss->deathSpells[j].position = (Vector2){ player->position.x + offset, player->groundY };
                                boss->deathSpells[j].velocity = (Vector2){ 0, 0 };
                                boss->deathSpells[j].radius = 40.0f;
                                boss->deathSpells[j].damage = 25.0f;
                                boss->deathSpells[j].maxFrames = 16;
                                boss->deathSpells[j].currentFrame = 0;
                                boss->deathSpells[j].frameTimer = 0.0f;
                            }
                        }
                    }
                }
                
                if (boss->currentFrame >= maxFrames) {
                    boss->state = BOSS_STATE_IDLE;
                    boss->currentFrame = 0;
                    boss->isCasting = false;
                }
            }
            break;
            
        case BOSS_STATE_HURT:
            maxFrames = 3;
            animSpeed = 0.1f;
            if (boss->frameTimer >= animSpeed) {
                boss->frameTimer = 0;
                boss->currentFrame++;
                if (boss->currentFrame >= maxFrames) {
                    boss->state = BOSS_STATE_IDLE;
                    boss->currentFrame = 0;
                    boss->isHurt = false;
                }
            }
            break;
            
        case BOSS_STATE_DEATH:
            maxFrames = 10;
            animSpeed = 0.15f;
            if (boss->frameTimer >= animSpeed) {
                boss->frameTimer = 0;
                if (boss->currentFrame < maxFrames - 1) {
                    boss->currentFrame++;
                } else {
                    boss->isAlive = false; // Kết thúc hoạt ảnh chết
                }
            }
            break;
    }

    // 2. Di chuyển AI & Quyết định đòn đánh (chỉ khi IDLE hoặc WALK)
    if (boss->state == BOSS_STATE_IDLE || boss->state == BOSS_STATE_WALK) {
        // Hướng nhìn của Boss luôn quay về phía Player
        boss->facingRight = (player->position.x > boss->position.x);

        float distToPlayer = player->position.x - boss->position.x;
        float distAbs = fabsf(distToPlayer);

        // Ngưỡng khoảng cách bắt đầu cast phép: Phase 1 (125.0f), Phase 2 chỉ thấp hơn một chút (110.0f)
        float castThreshold = (boss->currentPhase == 2) ? 110.0f : 125.0f;

        boss->attackTimer += dt;
        // Bỏ qua kiểm tra range cast phép nếu boss đang có khiên bảo vệ (forcefieldTimer > 0.0f)
        if (distAbs > castThreshold || boss->forcefieldTimer > 0.0f) {
            // Player ở xa ngưỡng hoặc boss có khiên -> Lập tức cast phép ép tiếp cận
            if (boss->attackTimer >= boss->attackCooldown) {
                boss->attackTimer = 0;
                boss->state = BOSS_STATE_CAST;
                boss->isCasting = true;
                boss->velocity.x = 0;
                boss->currentFrame = 0;
                boss->frameTimer = 0;
                
                if (boss->currentPhase == 1) {
                    // Phase 1: FireBomb 30%, DarkBolt 20%, Lightning 20%, DeathSpell (SPELL) 30%
                    int roll = GetRandomValue(0, 99);
                    if (roll < 30)       boss->attackPattern = SPELL_FIRE_BOMB;    // 30%
                    else if (roll < 50)  boss->attackPattern = SPELL_DARK_BOLT;    // 20%
                    else if (roll < 70)  boss->attackPattern = SPELL_LIGHTNING;    // 20%
                    else                 boss->attackPattern = SPELL_DEATH_SPELL;  // 30% (tăng từ 15%)
                } else {
                    // Phase 2: FireBomb 20%, DarkBolt 20%, Lightning 20%, DeathSpell (SPELL) 40%
                    int roll = GetRandomValue(0, 99);
                    if (roll < 20)       boss->attackPattern = SPELL_FIRE_BOMB;    // 20%
                    else if (roll < 40)  boss->attackPattern = SPELL_DARK_BOLT;    // 20%
                    else if (roll < 60)  boss->attackPattern = SPELL_LIGHTNING;    // 20%
                    else                 boss->attackPattern = SPELL_DEATH_SPELL;  // 40% (tăng từ 25%)
                }
            } else {
                // Đang trong thời gian hồi chiêu -> Đi bộ lại gần player
                boss->state = BOSS_STATE_WALK;
                float moveDir = (distToPlayer > 0) ? 1.0f : -1.0f;
                float walkSpeed = (boss->currentPhase == 2) ? 185.0f : 180.0f;
                if (boss->rageActive) walkSpeed *= 1.15f;
                boss->velocity.x = moveDir * walkSpeed;
                boss->position.x += boss->velocity.x * dt;
            }
        } else if (distAbs < 130.0f) {
            // Player ở quá gần -> Đập kiếm chém (Chiêu 1)
            if (boss->attackTimer >= boss->attackCooldown) {
                boss->state = BOSS_STATE_ATTACK;
                boss->currentFrame = 0;
                boss->frameTimer = 0;
                boss->attackTimer = 0;
                boss->isAttacking = true;
                boss->velocity.x = 0;
                boss->hasCheckedBehind = false; // Reset kiểm tra né sau lưng
            } else {
                // Đang trong thời gian hồi chiêu nhưng ở gần -> TIẾP TỤC ĐI BỘ VỀ PHÍA PLAYER để ép góc
                boss->state = BOSS_STATE_WALK;
                float moveDir = (distToPlayer > 0) ? 1.0f : -1.0f;
                float walkSpeed = (boss->currentPhase == 2) ? 185.0f : 180.0f;
                if (boss->rageActive) walkSpeed *= 1.15f;
                boss->velocity.x = moveDir * walkSpeed;
                boss->position.x += boss->velocity.x * dt;
            }
        } else {
            // Player ở khoảng giữa -> Đi bộ tiếp cận
            boss->state = BOSS_STATE_WALK;
            float moveDir = (distToPlayer > 0) ? 1.0f : -1.0f;
            float walkSpeed = (boss->currentPhase == 2) ? 185.0f : 180.0f;
            if (boss->rageActive) walkSpeed *= 1.15f;
            boss->velocity.x = moveDir * walkSpeed;
            boss->position.x += boss->velocity.x * dt;
        }
    }

    // 3. Cập nhật Projectile / Spells
    if (boss->projectile.active) {
        boss->projectile.frameTimer += dt;
        
        if (boss->projectile.type == SPELL_DARK_BOLT) {
            boss->projectile.position.x += boss->projectile.velocity.x * dt;
            boss->projectile.position.y += boss->projectile.velocity.y * dt;
            
            if (boss->projectile.frameTimer >= 0.08f) {
                boss->projectile.frameTimer = 0.0f;
                boss->projectile.currentFrame = (boss->projectile.currentFrame + 1) % boss->projectile.maxFrames;
            }

            if (fabsf(boss->projectile.position.x - boss->position.x) > 1200.0f) {
                boss->projectile.active = false;
            }

            float distToPlayer = Vector2Distance(boss->projectile.position, (Vector2){ player->position.x, player->position.y - 32.0f });
            if (distToPlayer < (boss->projectile.radius + 20.0f)) {
                boss->projectile.active = false;
                if (player->freezeTimer <= 0) {
                    player->freezeTimer = 0.3f;
                    player->currentHP -= 1.0f;
                            player->isHurt = true;
                            player->currentFrame = 0;
                            player->frameTimer = 0.0f;
                            player->freezeTimer = 0.2f; // Mất 1 máu
                    float pushDir = (boss->projectile.velocity.x > 0) ? 1.0f : -1.0f;
                    player->position.x += pushDir * 40.0f;
                    CameraShake(cam, 0.25f, 6.0f);
                    TraceLog(LOG_INFO, "Player hit by Boss DarkBolt spell! Player HP: %.1f", player->currentHP);
                }
            }
        }
        else if (boss->projectile.type == SPELL_FIRE_BOMB) {
            if (boss->projectile.state == 0) { // Giai đoạn cảnh báo Telegraph (16 frames * 0.075s = 1.2 giây)
                if (boss->projectile.frameTimer >= 0.075f) {
                    boss->projectile.frameTimer = 0.0f;
                    boss->projectile.currentFrame++;
                    if (boss->projectile.currentFrame >= boss->projectile.maxFrames) {
                        // Chuyển sang State 1 (Bùng nổ)
                        boss->projectile.state = 1;
                        boss->projectile.currentFrame = 0;
                        boss->projectile.frameTimer = 0.0f;
                        boss->projectile.maxFrames = 15; // 15 frames của Fire-bomb
                        
                        if (boss->currentPhase == 3) {
                            // Kích hoạt 2 đạn extraProjectiles bay sang trái và phải phân tách từ FireBomb
                            for (int i = 0; i < 2; i++) {
                                boss->extraProjectiles[i].active = true;
                                boss->extraProjectiles[i].type = SPELL_DARK_BOLT;
                                boss->extraProjectiles[i].position = boss->projectile.position;
                                boss->extraProjectiles[i].velocity = (Vector2){ (i == 0 ? -350.0f : 350.0f), 0.0f };
                                boss->extraProjectiles[i].radius = 15.0f;
                                boss->extraProjectiles[i].damage = 10.0f;
                                boss->extraProjectiles[i].maxFrames = 12;
                                boss->extraProjectiles[i].currentFrame = 0;
                                boss->extraProjectiles[i].frameTimer = 0.0f;
                            }
                        }
                    }
                }
            }
            else { // Giai đoạn bùng nổ cột lửa
                if (boss->projectile.frameTimer >= 0.07f) {
                    boss->projectile.frameTimer = 0.0f;
                    boss->projectile.currentFrame++;
                    if (boss->projectile.currentFrame >= boss->projectile.maxFrames) {
                        boss->projectile.active = false;
                    }
                }
                
                // Gây sát thương ở các frame vụ nổ mạnh nhất (frame 5-11)
                if (boss->projectile.currentFrame >= 5 && boss->projectile.currentFrame <= 11) {
                    float distH = fabsf(player->position.x - boss->projectile.position.x);
                    float distV = fabsf(player->position.y - boss->projectile.position.y);
                    
                    // Hitbox: bán kính ngang 50px (đường kính 100px), chiều cao cột lửa 80px
                    if (distH < boss->projectile.radius && distV < 80.0f) {
                        if (player->freezeTimer <= 0) {
                            player->freezeTimer = 0.45f; // Khựng mạnh
                            player->currentHP -= 1.0f;
                            player->isHurt = true;
                            player->currentFrame = 0;
                            player->frameTimer = 0.0f;
                            player->freezeTimer = 0.2f; // Mất 1 máu
                            float pushDir = (player->position.x > boss->projectile.position.x) ? 1.0f : -1.0f;
                            player->position.x += pushDir * 80.0f; // Bị đẩy lùi mạnh
                            CameraShake(cam, 0.35f, 9.0f);
                            TraceLog(LOG_INFO, "Player hit by Boss Ground FireBomb explosion! Player HP: %.1f", player->currentHP);
                        }
                    }
                }
            }
        }
        else if (boss->projectile.type == SPELL_LIGHTNING) {
            boss->projectile.stateTimer += dt;
            if (boss->projectile.state == 0) { // Cảnh báo sét đánh (nhanh hơn: 0.25s)
                if (boss->projectile.stateTimer >= 0.25f) {
                    boss->projectile.state = 1;
                    boss->projectile.stateTimer = 0.0f;
                    boss->projectile.currentFrame = 0;
                    boss->projectile.frameTimer = 0.0f;
                }
            }
            else { // Sét đánh xuống
                if (boss->projectile.frameTimer >= 0.06f) {
                    boss->projectile.frameTimer = 0.0f;
                    boss->projectile.currentFrame++;
                    if (boss->projectile.currentFrame >= boss->projectile.maxFrames) {
                        boss->projectile.active = false;
                        
                        // Khi sét 1 biến mất, luôn kích hoạt sét 2 khóa theo vị trí mới của Player!
                        boss->lightningStrikes[0].active = true;
                        boss->lightningStrikes[0].type = SPELL_LIGHTNING;
                        boss->lightningStrikes[0].state = 0; // Cảnh báo sét 2
                        boss->lightningStrikes[0].stateTimer = 0.0f;
                        boss->lightningStrikes[0].position = (Vector2){ player->position.x, player->position.y };
                        boss->lightningStrikes[0].radius = 30.0f;
                        boss->lightningStrikes[0].damage = 20.0f;
                        boss->lightningStrikes[0].maxFrames = 11;
                        boss->lightningStrikes[0].currentFrame = 0;
                        boss->lightningStrikes[0].frameTimer = 0.0f;
                    }
                }
                
                // Gây dam ở frame 2-6
                if (boss->projectile.currentFrame >= 2 && boss->projectile.currentFrame <= 6) {
                    float distToPlayerX = fabsf(player->position.x - boss->projectile.position.x);
                    if (distToPlayerX < boss->projectile.radius) {
                        if (player->freezeTimer <= 0) {
                            player->freezeTimer = 0.4f;
                            player->currentHP -= 1.0f;
                            player->isHurt = true;
                            player->currentFrame = 0;
                            player->frameTimer = 0.0f;
                            player->freezeTimer = 0.2f; // Mất 1 máu
                            CameraShake(cam, 0.4f, 12.0f);
                            TraceLog(LOG_INFO, "Player hit by Boss Lightning! Player HP: %.1f", player->currentHP);
                        }
                    }
                }
            }
        }
        else if (boss->projectile.type == SPELL_DEATH_SPELL) {
            boss->projectile.stateTimer += dt;
            if (boss->projectile.state == 0) { // Cảnh báo
                if (boss->projectile.stateTimer >= 0.6f) {
                    boss->projectile.state = 1;
                    boss->projectile.stateTimer = 0.0f;
                    boss->projectile.currentFrame = 0;
                    boss->projectile.frameTimer = 0.0f;
                }
            }
            else { // Phun trào (tốc độ x2: 0.07 → 0.035)
                if (boss->projectile.frameTimer >= 0.035f) {
                    boss->projectile.frameTimer = 0.0f;
                    boss->projectile.currentFrame++;
                    if (boss->projectile.currentFrame >= boss->projectile.maxFrames) {
                        boss->projectile.active = false;
                    }
                }
                
                // Gây dam từ frame 4-12
                if (boss->projectile.currentFrame >= 4 && boss->projectile.currentFrame <= 12) {
                    float distToPlayerX = fabsf(player->position.x - boss->projectile.position.x);
                    if (distToPlayerX < boss->projectile.radius) {
                        if (player->freezeTimer <= 0) {
                            player->freezeTimer = 0.3f;
                            player->currentHP -= 1.0f;
                            player->isHurt = true;
                            player->currentFrame = 0;
                            player->frameTimer = 0.0f;
                            player->freezeTimer = 0.2f; // Mất 1 máu
                            CameraShake(cam, 0.3f, 8.0f);
                            TraceLog(LOG_INFO, "Player hit by Boss DeathSpell! Player HP: %.1f", player->currentHP);
                        }
                    }
                }
            }
        }
    }

    // 3.1. Cập nhật extraProjectiles (Đạn phân mảnh từ FireBomb)
    for (int i = 0; i < 2; i++) {
        if (boss->extraProjectiles[i].active) {
            boss->extraProjectiles[i].position.x += boss->extraProjectiles[i].velocity.x * dt;
            boss->extraProjectiles[i].position.y += boss->extraProjectiles[i].velocity.y * dt;
            
            boss->extraProjectiles[i].frameTimer += dt;
            if (boss->extraProjectiles[i].frameTimer >= 0.08f) {
                boss->extraProjectiles[i].frameTimer = 0.0f;
                boss->extraProjectiles[i].currentFrame = (boss->extraProjectiles[i].currentFrame + 1) % boss->extraProjectiles[i].maxFrames;
            }
            
            // Tự hủy nếu bay quá xa khỏi vị trí ban đầu của Boss
            if (fabsf(boss->extraProjectiles[i].position.x - boss->position.x) > 1200.0f) {
                boss->extraProjectiles[i].active = false;
            }
            
            // Va chạm với Player
            float distToPlayer = Vector2Distance(boss->extraProjectiles[i].position, (Vector2){ player->position.x, player->position.y - 32.0f });
            if (distToPlayer < (boss->extraProjectiles[i].radius + 20.0f)) {
                boss->extraProjectiles[i].active = false;
                if (player->freezeTimer <= 0) {
                    player->freezeTimer = 0.25f;
                    player->currentHP -= 1.0f;
                            player->isHurt = true;
                            player->currentFrame = 0;
                            player->frameTimer = 0.0f;
                            player->freezeTimer = 0.2f; // Mất 1 máu
                    float pushDir = (boss->extraProjectiles[i].velocity.x > 0) ? 1.0f : -1.0f;
                    player->position.x += pushDir * 30.0f;
                    CameraShake(cam, 0.2f, 4.0f);
                    TraceLog(LOG_INFO, "Player hit by Boss split DarkBolt! Player HP: %.1f", player->currentHP);
                }
            }
        }
    }
    
    // 3.2. Cập nhật mảng lightningStrikes (Các tia sét đánh đồng loạt - Thiên Lôi Phạt)
    for (int i = 0; i < 4; i++) {
        if (boss->lightningStrikes[i].active) {
            boss->lightningStrikes[i].stateTimer += dt;
            
            // Cảnh báo (Telegraph) kéo dài 0.8 giây
            if (boss->lightningStrikes[i].state == 0) {
                if (boss->lightningStrikes[i].stateTimer >= 0.8f) {
                    boss->lightningStrikes[i].state = 1;
                    boss->lightningStrikes[i].stateTimer = 0.0f;
                    boss->lightningStrikes[i].currentFrame = 0;
                    boss->lightningStrikes[i].frameTimer = 0.0f;
                }
            }
            else { // Sét đánh xuống
                boss->lightningStrikes[i].frameTimer += dt;
                if (boss->lightningStrikes[i].frameTimer >= 0.06f) {
                    boss->lightningStrikes[i].frameTimer = 0.0f;
                    boss->lightningStrikes[i].currentFrame++;
                    if (boss->lightningStrikes[i].currentFrame >= boss->lightningStrikes[i].maxFrames) {
                        boss->lightningStrikes[i].active = false;
                    }
                }
                
                // Va chạm sét với Player (Bức tường dọc chịu sát thương mọi độ cao)
                if (boss->lightningStrikes[i].currentFrame >= 2 && boss->lightningStrikes[i].currentFrame <= 8) {
                    float distH = fabsf(player->position.x - boss->lightningStrikes[i].position.x);
                    if (distH < boss->lightningStrikes[i].radius) {
                        if (player->freezeTimer <= 0) {
                            player->freezeTimer = 0.45f;
                            player->currentHP -= 1.0f;
                            player->isHurt = true;
                            player->currentFrame = 0;
                            player->frameTimer = 0.0f;
                            player->freezeTimer = 0.2f; // Mất 1 máu
                            float pushDir = (player->position.x > boss->lightningStrikes[i].position.x) ? 1.0f : -1.0f;
                            player->position.x += pushDir * 50.0f; // Bị giật đẩy lùi
                            CameraShake(cam, 0.35f, 10.0f);
                            TraceLog(LOG_INFO, "Player hit by Boss Lightning Rain! Player HP: %.1f", player->currentHP);
                        }
                    }
                }
            }
        }
    }

    // 3.2.1. Cập nhật Ma Thuật Hộ Thân (Chiêu 3) và các tia đạn bám đuổi (Spark)
    if (boss->auraActive) {
        boss->auraTimer -= dt;
        if (boss->auraTimer <= 0.0f) {
            boss->auraActive = false;
        }
    }

    if (boss->auraActive) {
        
        // Mỗi 1.25 giây bắn một tia đạn bám đuổi (sparks)
        boss->auraSparkTimer += dt;
        if (boss->auraSparkTimer >= 1.25f) {
            boss->auraSparkTimer = 0.0f;
            
            // Đếm số lượng đạn sparks đang kích hoạt
            int activeCount = 0;
            for (int i = 0; i < 4; i++) {
                if (boss->auraSparks[i].active) activeCount++;
            }

            if (activeCount < 2) {
                // Tìm slot trống trong mảng auraSparks
                for (int i = 0; i < 4; i++) {
                    if (!boss->auraSparks[i].active) {
                        boss->auraSparks[i].active = true;
                        boss->auraSparks[i].type = SPELL_DARK_BOLT;
                        // Bắn ra từ tâm Boss
                        boss->auraSparks[i].position = (Vector2){ boss->position.x, boss->position.y - 30.0f };
                        
                        Vector2 target = { player->position.x, player->position.y - 32.0f };
                        Vector2 diff = Vector2Subtract(target, boss->auraSparks[i].position);
                        Vector2 dir = Vector2Normalize(diff);
                        float speed = 180.0f; // Tăng tốc độ bay ban đầu
                        boss->auraSparks[i].velocity = (Vector2){ dir.x * speed, dir.y * speed };
                        boss->auraSparks[i].radius = 15.0f;
                        boss->auraSparks[i].damage = 10.0f;
                        boss->auraSparks[i].maxFrames = 7; // spark có 7 frames hoạt ảnh
                        boss->auraSparks[i].currentFrame = 0;
                        boss->auraSparks[i].frameTimer = 0.0f;
                        break;
                    }
                }
            }
        }
    }
    
    // Cập nhật các đạn bám đuôi auraSparks
    for (int i = 0; i < 4; i++) {
        if (boss->auraSparks[i].active) {
            // Homing/Steering logic: xoay chuyển từ từ hướng về mục tiêu
            Vector2 target;
            float speed;
            float lerpWeight;
            if (boss->auraSparks[i].state == 99) {
                target = (Vector2){ boss->position.x, boss->position.y - 45.0f };
                speed = 500.0f; // Bay về rất nhanh
                lerpWeight = 12.0f; // Bẻ lái cực gắt về phía boss
            } else {
                target = (Vector2){ player->position.x, player->position.y - 32.0f };
                speed = 180.0f; // Tăng từ 80.0f để bám đuổi nhanh hơn
                lerpWeight = 6.0f; // Tăng từ 3.0f để bám đuổi sát hơn
            }
            
            Vector2 diff = Vector2Subtract(target, boss->auraSparks[i].position);
            float dist = Vector2Length(diff);
            if (dist > 5.0f) {
                Vector2 dir = Vector2Normalize(diff);
                boss->auraSparks[i].velocity.x = Lerp(boss->auraSparks[i].velocity.x, dir.x * speed, lerpWeight * dt);
                boss->auraSparks[i].velocity.y = Lerp(boss->auraSparks[i].velocity.y, dir.y * speed, lerpWeight * dt);
            }
            
            // Di chuyển đạn
            boss->auraSparks[i].position.x += boss->auraSparks[i].velocity.x * dt;
            boss->auraSparks[i].position.y += boss->auraSparks[i].velocity.y * dt;
            
            // Cập nhật hoạt ảnh
            boss->auraSparks[i].frameTimer += dt;
            if (boss->auraSparks[i].frameTimer >= 0.08f) {
                boss->auraSparks[i].frameTimer = 0.0f;
                boss->auraSparks[i].currentFrame = (boss->auraSparks[i].currentFrame + 1) % boss->auraSparks[i].maxFrames;
            }
            
            // Tự hủy nếu bay quá xa khỏi biên đấu trường
            if (boss->auraSparks[i].position.x < 0.0f || boss->auraSparks[i].position.x > 2500.0f) {
                boss->auraSparks[i].active = false;
            }
            
            // Va chạm với Player (Chỉ gây dmg khi chưa bị phản đạn)
            if (boss->auraSparks[i].state != 99) {
                float distToPlayer = Vector2Distance(boss->auraSparks[i].position, (Vector2){ player->position.x, player->position.y - 32.0f });
                if (distToPlayer < (boss->auraSparks[i].radius + 20.0f)) {
                    boss->auraSparks[i].active = false;
                    if (player->freezeTimer <= 0) {
                        player->freezeTimer = 0.25f;
                        player->currentHP -= 1.0f;
                        player->isHurt = true;
                        player->currentFrame = 0;
                        player->frameTimer = 0.0f;
                        player->freezeTimer = 0.2f; // Mất 1 máu
                        float pushDir = (boss->auraSparks[i].velocity.x > 0) ? 1.0f : -1.0f;
                        player->position.x += pushDir * 35.0f;
                        CameraShake(cam, 0.2f, 5.0f);
                        TraceLog(LOG_INFO, "Player hit by Boss Spark (Homing DarkBolt)! Player HP: %.1f", player->currentHP);
                    }
                }
            } else {
                // Va chạm với Boss (Nếu đạn đã bị phản lại)
                float distToBoss = Vector2Distance(boss->auraSparks[i].position, (Vector2){ boss->position.x, boss->position.y - 45.0f });
                if (distToBoss < (boss->auraSparks[i].radius + 45.0f)) {
                    boss->auraSparks[i].active = false;
                    if (boss->state != BOSS_STATE_DEATH) {
                        boss->currentHP -= 4.0f; // Sát thương to gấp 2 lần chém thường (Xuyên Giáp/Khiên)
                        if (boss->currentHP < 0) boss->currentHP = 0;
                        
                        if (boss->state != BOSS_STATE_ATTACK && boss->state != BOSS_STATE_CAST) {
                            boss->state = BOSS_STATE_HURT;
                            boss->currentFrame = 0;
                            boss->frameTimer = 0.0f;
                            boss->isHurt = true;
                            float kb = (boss->auraSparks[i].velocity.x > 0) ? 20.0f : -20.0f;
                            boss->position.x += kb;
                        }
                        CameraShake(cam, 0.3f, 8.0f); // Rung lắc cực mạnh khi trúng phản đạn
                        TraceLog(LOG_INFO, "Boss hit by reflected Spark ball (Shield Pierced)! DMG: 4.0. Current HP: %.1f", boss->currentHP);
                    }
                }
            }
        }
    }

    // 3.3. Cập nhật deathSpells[0] và deathSpells[1] (Hai cột phép phụ hai bên ở Phase 3)
    for (int i = 0; i < 2; i++) {
        if (boss->deathSpells[i].active) {
            boss->deathSpells[i].stateTimer += dt;
            if (boss->deathSpells[i].state == 0) { // Cảnh báo
                if (boss->deathSpells[i].stateTimer >= 0.6f) {
                    boss->deathSpells[i].state = 1;
                    boss->deathSpells[i].stateTimer = 0.0f;
                    boss->deathSpells[i].currentFrame = 0;
                    boss->deathSpells[i].frameTimer = 0.0f;
                }
            }
            else { // Phun trào
                boss->deathSpells[i].frameTimer += dt;
                if (boss->deathSpells[i].frameTimer >= 0.07f) {
                    boss->deathSpells[i].frameTimer = 0.0f;
                    boss->deathSpells[i].currentFrame++;
                    if (boss->deathSpells[i].currentFrame >= boss->deathSpells[i].maxFrames) {
                        boss->deathSpells[i].active = false;
                    }
                }
                
                // Va chạm
                if (boss->deathSpells[i].currentFrame >= 4 && boss->deathSpells[i].currentFrame <= 12) {
                    float distToPlayerX = fabsf(player->position.x - boss->deathSpells[i].position.x);
                    if (distToPlayerX < boss->deathSpells[i].radius) {
                        if (player->freezeTimer <= 0) {
                            player->freezeTimer = 0.3f;
                            player->currentHP -= 1.0f;
                            player->isHurt = true;
                            player->currentFrame = 0;
                            player->frameTimer = 0.0f;
                            player->freezeTimer = 0.2f; // Mất 1 máu
                            CameraShake(cam, 0.25f, 6.0f);
                            TraceLog(LOG_INFO, "Player hit by Boss extra DeathSpell column! Player HP: %.1f", player->currentHP);
                        }
                    }
                }
            }
        }
    }

    // 3.4. Kiểm tra chém phản đạn Spark Ball (Phase 2)
    if (player->isAttacking && player->currentFrame >= 2 && player->currentFrame <= 5) {
        for (int i = 0; i < 4; i++) {
            if (boss->auraSparks[i].active && boss->auraSparks[i].state != 99) {
                Vector2 swordCenter = { player->position.x + (player->facingRight ? 45.0f : -45.0f), player->position.y - 32.0f };
                float distToSpark = Vector2Distance(swordCenter, boss->auraSparks[i].position);
                
                // Hitbox phản tương tác to gấp đôi (bán kính = 30.0f, tầm kiếm = 40.0f)
                if (distToSpark < (30.0f + 40.0f)) {
                    boss->auraSparks[i].state = 99; // Đánh dấu đã bị phản đạn
                    
                    Vector2 bossCenter = { boss->position.x, boss->position.y - 45.0f };
                    Vector2 toBoss = Vector2Subtract(bossCenter, boss->auraSparks[i].position);
                    Vector2 dir = Vector2Normalize(toBoss);
                    float reflectSpeed = 500.0f;
                    boss->auraSparks[i].velocity = (Vector2){ dir.x * reflectSpeed, dir.y * reflectSpeed };
                    
                    CameraShake(cam, 0.15f, 4.0f);
                    TraceLog(LOG_INFO, "Player deflected Boss Spark ball!");
                }
            }
        }
    }

    // 4. Kiểm tra va chạm với đòn đánh của Player
    if (player->isAttacking && player->currentFrame >= 2 && player->currentFrame <= 5) {
        float attackDist = 70.0f;
        float distToBoss = player->position.x - boss->position.x;
        float distAbs = fabsf(distToBoss);
        
        // Kiểm tra xem player có quay mặt đúng hướng boss không
        bool correctDirection = (player->facingRight && distToBoss < 0) || (!player->facingRight && distToBoss > 0);

        if (distAbs < attackDist && correctDirection && boss->state != BOSS_STATE_HURT && boss->state != BOSS_STATE_DEATH && boss->invincibilityTimer <= 0.0f) {
            boss->currentHP -= 2.5f; // Sát thương của mèo là 2.5 dmg
            if (boss->currentHP < 0) boss->currentHP = 0;
            boss->invincibilityTimer = 0.8f; // Bất tử trong 0.8 giây sau khi dính đòn (I-frames tránh spam)

            // Đếm số đòn đánh liên tiếp để kích hoạt chống cheese
            boss->consecutiveHits++;
            boss->hitResetTimer = 3.0f; // Reset sau 3 giây không bị chém
            
            if (boss->consecutiveHits >= 3) {
                boss->consecutiveHits = 0;
                
                // Nếu ở Phase 2 và chưa nộ, kích hoạt cả Rage Mode
                if (boss->currentPhase == 2 && !boss->rageActive) {
                    boss->rageActive = true;
                    boss->rageTimer = 5.0f;
                    TraceLog(LOG_INFO, "Boss entered RAGE MODE after 3 consecutive hits!");
                }
                
                // Kích hoạt chiêu thức Phép Đẩy Gió (Wind Push) để giãn khoảng cách
                boss->state = BOSS_STATE_CAST;
                boss->currentFrame = 0;
                boss->frameTimer = 0.0f;
                boss->isCasting = true;
                boss->isAttacking = false;
                boss->isHurt = false;
                boss->attackPattern = 99; // Mã 99 cho Wind Push
                
                TraceLog(LOG_INFO, "Boss triggers Wind Push counter-move to prevent cheese!");
            }

            // Chỉ interrupt sang HURT nếu Boss KHÔNG đang tấn công/cast
            // (Animation ATTACK và CAST không thể bị cancel)
            if (boss->state != BOSS_STATE_ATTACK && boss->state != BOSS_STATE_CAST) {
                boss->state = BOSS_STATE_HURT;
                boss->currentFrame = 0;
                boss->frameTimer = 0.0f;
                boss->isHurt = true;
                // Đẩy lùi boss ra xa (Knockback away from player)
                float kb = (player->facingRight) ? 30.0f : -30.0f;
                boss->position.x += kb;
            }
            CameraShake(cam, 0.2f, 5.0f); // Rung lắc khi chém trúng Boss
            TraceLog(LOG_INFO, "Boss hit by Player! Current HP: %.1f (Consecutive: %d)", boss->currentHP, boss->consecutiveHits);
        }
    }

    // 5. Solid Body Collision: Mèo không thể đi xuyên qua Boss
    {
        float minSep = 28.3f; // Khoảng cách tối thiểu giảm xuống còn 1/3 (85/3) để Mèo dễ chém trúng
        float dx = player->position.x - boss->position.x;
        float absDx = fabsf(dx);

        // "Nhảy qua đầu Boss": đang nhảy VÀ đã lên đủ cao hơn đầu Boss
        // jumpForce = -1200, đỉnh nhảy ≈ 180px → đầu Boss ≈ boss.y - 160px
        // Threshold thấp hơn để dễ nhảy qua (100px thay vì 120px)
        bool jumpingOver = player->isJumping && (player->position.y < boss->position.y - 100.0f);

        if (absDx < minSep && !jumpingOver) {
            // Đẩy Mèo ra đúng khoảng cách tối thiểu theo hướng hiện tại
            float pushDir = (dx >= 0.0f) ? 1.0f : -1.0f;
            player->position.x = boss->position.x + pushDir * minSep;
        }
    }

    // Arena clamp cho PLAYER chạy SAU collision để không bị xung đột thứ tự
    if (player->controlsEnabled && player->currentHP > 0.0f) {
        if (player->position.x < 35.0f) player->position.x = 35.0f;
        if (player->position.x > 1315.0f) player->position.x = 1315.0f;
    }

    // Giới hạn khu vực đấu boss (Dead Cells style) - Boss
    if (boss->position.x < 35.0f) boss->position.x = 35.0f;
    if (boss->position.x > 1315.0f) boss->position.x = 1315.0f;
}

void DrawBoss(Boss *boss, float scale) {
    if (!boss->isAlive && boss->state == BOSS_STATE_DEATH && boss->currentFrame >= 9) return;

    Texture2D currentTex;
    // int maxFrames = 8; // unused
    
    switch (boss->state) {
        case BOSS_STATE_IDLE:
            currentTex = texBossIdle[boss->currentFrame % 8];
            break;
        case BOSS_STATE_WALK:
            currentTex = texBossWalk[boss->currentFrame % 8];
            break;
        case BOSS_STATE_ATTACK:
            currentTex = texBossAttack[boss->currentFrame % 10];
            break;
        case BOSS_STATE_CAST:
            currentTex = texBossCast[boss->currentFrame % 9];
            break;
        case BOSS_STATE_HURT:
            currentTex = texBossHurt[boss->currentFrame % 3];
            break;
        case BOSS_STATE_DEATH:
            currentTex = texBossDeath[boss->currentFrame % 10];
            break;
        default:
            currentTex = texBossIdle[0];
            break;
    }

    float frameW = 140.0f;
    float frameH = 93.0f;
    
    Rectangle source = { 0, 0, frameW, frameH };
    
    // Nếu quay về bên phải, chúng ta flip texture bằng cách đổi chiều source.width
    // Bringer of Death mặc định quay mặt về bên trái.
    if (boss->facingRight) {
        source.width = -source.width;
    }

    // Căn lề bottom-center cho Boss tương tự như Player
    // Vì chiều cao Boss là 93px (lớn hơn 64px của mèo), yOffset sẽ điều chỉnh để đứng chính xác trên đất
    float yOffset = 20.0f; 

    // Bù đắp độ lệch tâm của Sprite (Sprite offset compensation) để tránh Boss bị dịch chuyển/teleport khi quay đầu
    float baseOffset = 35.0f;
    switch (boss->state) {
        case BOSS_STATE_IDLE:   baseOffset = 34.75f; break;
        case BOSS_STATE_WALK:   baseOffset = 31.31f; break;
        case BOSS_STATE_ATTACK: baseOffset = 16.00f; break;
        case BOSS_STATE_CAST:   baseOffset = 29.89f; break;
        case BOSS_STATE_HURT:   baseOffset = 35.33f; break;
        case BOSS_STATE_DEATH:  baseOffset = 37.20f; break;
        default:                baseOffset = 35.00f; break;
    }
    float xOffset = boss->facingRight ? (baseOffset * scale) : -(baseOffset * scale);

    Rectangle dest = { 
        roundf(boss->position.x + xOffset), 
        roundf(boss->position.y + yOffset), 
        frameW * scale, 
        frameH * scale 
    };
    // Điểm tựa (origin) ở đáy giữa khung hình
    Vector2 origin = { (frameW * scale) / 2.0f, frameH * scale };

    // Vẽ Boss với hiệu ứng chớp đỏ nếu bị đau hoặc rực đỏ giận dữ ở Phase 3 (Berserk)
    Color tint = WHITE;
    if (boss->state == BOSS_STATE_HURT) {
        tint = (Color){ 255, 120, 120, 255 };
    } else if (boss->currentPhase == 3) {
        // Nhấp nháy sắc đỏ giận dữ theo nhịp thở (sin wave)
        unsigned char redPulse = (unsigned char)(190 + 65.0f * sinf(GetTime() * 8.0f));
        tint = (Color){ 255, redPulse, redPulse, 255 };
    }

    DrawTexturePro(currentTex, source, dest, origin, 0.0f, tint);

    // Vẽ Projectile nếu đang hoạt động
    if (boss->projectile.active) {
        if (boss->projectile.type == SPELL_DARK_BOLT) {
            Texture2D projTex = texSpellDarkBolt[boss->projectile.currentFrame % 12];
            Rectangle projSource = { 0, 0, (float)projTex.width, (float)projTex.height };
            if (boss->projectile.velocity.x > 0) {
                projSource.width = -projSource.width; // Flip theo chiều bay
            }
            
            Rectangle projDest = {
                boss->projectile.position.x,
                boss->projectile.position.y,
                (float)projTex.width * 1.5f,
                (float)projTex.height * 1.5f
            };
            Vector2 projOrigin = { ((float)projTex.width * 1.5f) / 2.0f, ((float)projTex.height * 1.5f) / 2.0f };
            DrawTexturePro(projTex, projSource, projDest, projOrigin, 0.0f, WHITE);
        }
        else if (boss->projectile.type == SPELL_FIRE_BOMB) {
            if (boss->projectile.state == 0) {
                // Vẽ vòng tròn ma thuật tím cảnh báo (Bringer-of-Death_Spell) dưới mặt đất
                Texture2D warningTex = texSpellDeathSpell[boss->projectile.currentFrame % 16];
                Rectangle projSource = { 0, 0, (float)warningTex.width, (float)warningTex.height };
                float scaleMod = 2.0f;
                Rectangle projDest = {
                    boss->projectile.position.x,
                    boss->projectile.position.y, // Đặt thẳng xuống mặt đất
                    (float)warningTex.width * scaleMod,
                    (float)warningTex.height * scaleMod
                };
                // Quay 180 độ và đặt gốc tọa độ ở trên cùng (0) để "mở cổng" bắn ngược lên trời
                Vector2 projOrigin = { ((float)warningTex.width * scaleMod) / 2.0f, 0.0f };
                DrawTexturePro(warningTex, projSource, projDest, projOrigin, 180.0f, WHITE);
            }
            else {
                // Vẽ quả bom lửa bùng nổ (Fire-bomb)
                Texture2D projTex = texSpellFireBomb[boss->projectile.currentFrame % 15];
                Rectangle projSource = { 0, 0, (float)projTex.width, (float)projTex.height };
                
                // Tăng độ to thêm 15% cho Phase 1 (2.8f * 1.15 = 3.22f) và 20% cho Phase 2 (2.8f * 1.20 = 3.36f)
                float scaleMod = (boss->currentPhase == 2) ? 3.36f : 3.22f;
                float yOffset = (boss->currentPhase == 2) ? 30.0f : 28.75f;
                
                Rectangle projDest = {
                    boss->projectile.position.x,
                    boss->projectile.position.y - yOffset, // Căn lề nổ từ dưới đất trồi lên
                    (float)projTex.width * scaleMod,
                    (float)projTex.height * scaleMod
                };
                Vector2 projOrigin = { ((float)projTex.width * scaleMod) / 2.0f, ((float)projTex.height * scaleMod) / 2.0f };
                DrawTexturePro(projTex, projSource, projDest, projOrigin, 0.0f, WHITE);
            }
        }
        else if (boss->projectile.type == SPELL_LIGHTNING) {
            if (boss->projectile.state == 0) {
                // Vẽ chỉ báo cảnh báo sét đánh nhấp nháy đỏ trên mặt đất
                DrawCircle((int)boss->projectile.position.x, (int)boss->projectile.position.y, (int)boss->projectile.radius, (Color){ 230, 40, 40, 100 });
                DrawCircleLines((int)boss->projectile.position.x, (int)boss->projectile.position.y, (int)boss->projectile.radius, RED);
            } 
            else {
                // Vẽ cột sét thẳng đứng từ trên trời
                Texture2D projTex = texSpellLightning[boss->projectile.currentFrame % 11];
                Rectangle projSource = { 0, 0, (float)projTex.width, (float)projTex.height };
                
                float scaleX = 2.0f;
                // Kéo giãn theo chiều dọc đến mặt đất
                Rectangle projDest = {
                    boss->projectile.position.x,
                    0.0f,
                    (float)projTex.width * scaleX,
                    boss->projectile.position.y
                };
                Vector2 projOrigin = { ((float)projTex.width * scaleX) / 2.0f, 0.0f };
                DrawTexturePro(projTex, projSource, projDest, projOrigin, 0.0f, WHITE);
            }
        }
        else if (boss->projectile.type == SPELL_DEATH_SPELL) {
            if (boss->projectile.state == 0) {
                // Vẽ vòng tòa sáng nhỏ cảnh báo quanh đầu Boss
                DrawCircle((int)boss->projectile.position.x, (int)boss->projectile.position.y, (int)boss->projectile.radius, (Color){ 130, 30, 210, 80 });
                DrawCircleLines((int)boss->projectile.position.x, (int)boss->projectile.position.y, (int)boss->projectile.radius, PURPLE);
            }
            else {
                // Vẽ Death Spell gấp 3 lần to hơn, căn giữa theo đầu Boss
                Texture2D projTex = texSpellDeathSpell[boss->projectile.currentFrame % 16];
                Rectangle projSource = { 0, 0, (float)projTex.width, (float)projTex.height };
                
                float scaleMod = 7.5f; // Gấp 3 lần (từ 2.5f lên 7.5f)
                Rectangle projDest = {
                    boss->projectile.position.x,
                    boss->projectile.position.y,
                    (float)projTex.width * scaleMod,
                    (float)projTex.height * scaleMod
                };
                // Đặt Origin ở dưới cùng (đáy) để cột sét giáng từ trên trời xuống mặt đất
                Vector2 projOrigin = { ((float)projTex.width * scaleMod) / 2.0f, (float)projTex.height * scaleMod };
                DrawTexturePro(projTex, projSource, projDest, projOrigin, 0.0f, WHITE); // Không lật ngược nữa
            }
        }
    }

    // Vẽ các Projectile phụ phân mảnh từ FireBomb
    for (int i = 0; i < 2; i++) {
        if (boss->extraProjectiles[i].active) {
            Texture2D projTex = texSpellDarkBolt[boss->extraProjectiles[i].currentFrame % 12];
            Rectangle projSource = { 0, 0, (float)projTex.width, (float)projTex.height };
            if (boss->extraProjectiles[i].velocity.x > 0) {
                projSource.width = -projSource.width;
            }
            // Vẽ nhỏ hơn đạn chính một chút
            Rectangle projDest = {
                boss->extraProjectiles[i].position.x,
                boss->extraProjectiles[i].position.y,
                (float)projTex.width * 1.0f,
                (float)projTex.height * 1.0f
            };
            Vector2 projOrigin = { ((float)projTex.width * 1.0f) / 2.0f, ((float)projTex.height * 1.0f) / 2.0f };
            DrawTexturePro(projTex, projSource, projDest, projOrigin, 0.0f, WHITE);
        }
    }
    // (Đã xóa hiệu ứng Aura và Rage Aura theo yêu cầu)

    // Vẽ các đạn bám đuôi auraSparks (sử dụng spark loop thay vì DarkBolt)
    for (int i = 0; i < 4; i++) {
        if (boss->auraSparks[i].active) {
            Texture2D projTex = texSpellSpark[boss->auraSparks[i].currentFrame % 7];
            Rectangle projSource = { 0, 0, (float)projTex.width, (float)projTex.height };
            if (boss->auraSparks[i].velocity.x > 0) {
                projSource.width = -projSource.width;
            }
            float scale = 2.0f; // Scale lớn hơn chút cho dễ nhìn
            Rectangle projDest = {
                boss->auraSparks[i].position.x,
                boss->auraSparks[i].position.y,
                (float)projTex.width * scale,
                (float)projTex.height * scale
            };
            Vector2 projOrigin = { ((float)projTex.width * scale) / 2.0f, ((float)projTex.height * scale) / 2.0f };
            Color tint = (boss->auraSparks[i].state == 99) ? GOLD : WHITE;
            DrawTexturePro(projTex, projSource, projDest, projOrigin, 0.0f, tint);
        }
    }

    // Vẽ Thiên Lôi Phạt (mảng 4 tia sét giáng xuống đồng thời)
    for (int i = 0; i < 4; i++) {
        if (boss->lightningStrikes[i].active) {
            if (boss->lightningStrikes[i].state == 0) {
                // Vẽ vệt sáng laser dọc cảnh báo từ đỉnh màn hình xuống đất
                DrawRectangle((int)boss->lightningStrikes[i].position.x - 5, 0, 10, (int)boss->lightningStrikes[i].position.y, (Color){ 230, 40, 40, 95 });
                DrawLine((int)boss->lightningStrikes[i].position.x, 0, (int)boss->lightningStrikes[i].position.x, (int)boss->lightningStrikes[i].position.y, RED);
            }
            else {
                // Sét giáng xuống
                Texture2D projTex = texSpellLightning[boss->lightningStrikes[i].currentFrame % 11];
                Rectangle projSource = { 0, 0, (float)projTex.width, (float)projTex.height };
                float scaleX = 2.0f;
                Rectangle projDest = {
                    boss->lightningStrikes[i].position.x,
                    0.0f,
                    (float)projTex.width * scaleX,
                    boss->lightningStrikes[i].position.y
                };
                Vector2 projOrigin = { ((float)projTex.width * scaleX) / 2.0f, 0.0f };
                DrawTexturePro(projTex, projSource, projDest, projOrigin, 0.0f, WHITE);
            }
        }
    }
    // Vẽ Tam đại cột tối phụ (deathSpells[0] và deathSpells[1])
    for (int i = 0; i < 2; i++) {
        if (boss->deathSpells[i].active) {
            if (boss->deathSpells[i].state == 0) {
                DrawEllipse((int)boss->deathSpells[i].position.x, (int)boss->deathSpells[i].position.y, (int)boss->deathSpells[i].radius, 10, (Color){ 130, 30, 210, 120 });
                DrawEllipseLines((int)boss->deathSpells[i].position.x, (int)boss->deathSpells[i].position.y, (int)boss->deathSpells[i].radius, 10, PURPLE);
            }
            else {
                Texture2D projTex = texSpellDeathSpell[boss->deathSpells[i].currentFrame % 16];
                Rectangle projSource = { 0, 0, (float)projTex.width, (float)projTex.height };
                float scaleMod = 2.0f; // Hơi nhỏ hơn cột chính tí
                Rectangle projDest = {
                    boss->deathSpells[i].position.x,
                    boss->deathSpells[i].position.y + 10.0f,
                    (float)projTex.width * scaleMod,
                    (float)projTex.height * scaleMod
                };
                Vector2 projOrigin = { ((float)projTex.width * scaleMod) / 2.0f, (float)projTex.height * scaleMod }; // Đặt gốc ở dưới cùng để giáng từ trên xuống
                DrawTexturePro(projTex, projSource, projDest, projOrigin, 0.0f, WHITE);
            }
        }
    }

    // Vẽ luồng gió đẩy lùi (Wind Push) chống cheese
    if (boss->state == BOSS_STATE_CAST && boss->attackPattern == 99 && boss->currentFrame >= 5) {
        float radius = (float)(boss->currentFrame - 5) * 60.0f + 40.0f;
        float fade = 1.0f - (float)(boss->currentFrame - 5) / 4.0f;
        if (fade < 0.0f) fade = 0.0f;
        Color windColor = Fade(LIGHTGRAY, 0.4f * fade);
        DrawCircleLinesV((Vector2){ boss->position.x, boss->position.y - 45.0f }, radius, windColor);
        DrawCircleLinesV((Vector2){ boss->position.x, boss->position.y - 45.0f }, radius - 15.0f, Fade(LIGHTGRAY, 0.2f * fade));
    }

    // Vẽ Aura bất tử và cản vật lý (Phase 2 Concierge style forcefield - Elliptical)
    if (boss->forcefieldTimer > 0.0f) {
        float rH = 75.0f; // Bán kính ngang
        float rV = 85.0f; // Bán kính dọc (bé lại và cân đối hơn)
        float alpha = 0.25f + 0.15f * sinf(GetTime() * 12.0f); // Tạo hiệu ứng nhấp nháy/pulse nhanh hơn cho đẹp
        Vector2 center = { boss->position.x, boss->position.y - 45.0f };
        
        // Màu tím/hồng neon huyền ảo phù hợp Bringer of Death
        Color shieldFill = Fade((Color){ 180, 70, 255, 255 }, alpha);
        Color shieldBorder = (Color){ 200, 100, 255, 255 };
        Color shieldOuterBorder = Fade((Color){ 140, 50, 255, 255 }, 0.5f);
        
        // Vẽ khối e-líp mờ ảo phát sáng
        DrawEllipse((int)center.x, (int)center.y, rH, rV, shieldFill);
        
        // Vẽ các đường viền sắc nét lồng nhau tạo hiệu ứng 3D/hologram đẹp mắt
        DrawEllipseLines((int)center.x, (int)center.y, rH, rV, shieldBorder);
        DrawEllipseLines((int)center.x, (int)center.y, rH + 2.0f, rV + 2.0f, shieldOuterBorder);
        DrawEllipseLines((int)center.x, (int)center.y, rH - 2.0f, rV - 2.0f, Fade(shieldBorder, 0.4f));
        
        // Thêm hiệu ứng sét sét giật giật (Electric Sparks) bao quanh khiên
        int sparkCount = GetRandomValue(3, 6);
        for (int i = 0; i < sparkCount; i++) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float rOffset = (float)GetRandomValue(-5, 10);
            Vector2 start = { center.x + (rH + rOffset) * cosf(angle), center.y + (rV + rOffset) * sinf(angle) };
            
            float angle2 = angle + (float)GetRandomValue(-20, 20) * DEG2RAD;
            float rOffset2 = (float)GetRandomValue(-5, 15);
            Vector2 end = { center.x + (rH + rOffset2) * cosf(angle2), center.y + (rV + rOffset2) * sinf(angle2) };
            
            Color sparkColor = (GetRandomValue(0, 1) == 0) ? WHITE : (Color){ 200, 150, 255, 200 };
            DrawLineEx(start, end, (float)GetRandomValue(1, 3), sparkColor);
        }
    }
}

void DrawBossHP(Boss *boss) {
    if (!boss->isAlive) return;

    // Tọa độ vẽ thanh máu thanh thoát và cao cấp ở góc trên chính giữa màn hình
    int screenW = GetScreenWidth();
    int barW = 500;
    int barH = 14;
    int barX = (screenW - barW) / 2;
    int barY = 35;

    // Container nền mờ mượt mà (Glassmorphism effect)
    DrawRectangleRounded((Rectangle){ barX - 6, barY - 6, barW + 12, barH + 12 + 18 }, 0.25f, 4, (Color){ 20, 20, 25, 190 });
    DrawRectangleRoundedLines((Rectangle){ barX - 6, barY - 6, barW + 12, barH + 12 + 18 }, 0.25f, 4, 1.5f, (Color){ 100, 100, 110, 80 });

    // Thanh máu nền đen sâu
    DrawRectangle(barX, barY, barW, barH, BLACK);

    // Phần trăm máu hiện tại
    float hpPercent = boss->currentHP / boss->maxHP;
    if (hpPercent < 0.0f) hpPercent = 0.0f;
    
    // Vẽ phần máu đầy với màu đỏ rực rỡ gradient/sleek
    DrawRectangle(barX, barY, (int)(barW * hpPercent), barH, (Color){ 220, 30, 45, 255 });
    
    // Viền nhẹ cho thanh máu chính
    DrawRectangleLines(barX, barY, barW, barH, (Color){ 160, 160, 170, 120 });

    // Tên của Boss sang trọng và sắc sảo
    const char* bossName = "BRINGER OF DEATH";
    int fontSize = 14;
    int textW = MeasureText(bossName, fontSize);
    DrawText(bossName, barX + (barW - textW) / 2, barY + barH + 6, fontSize, (Color){ 245, 245, 250, 255 });
    
    // Điểm số HP bằng số nhỏ ở bên phải
    const char* hpText = TextFormat("%.0f / %.0f", boss->currentHP, boss->maxHP);
    DrawText(hpText, barX + barW - MeasureText(hpText, 10) - 4, barY + 2, 10, (Color){ 255, 255, 255, 200 });
}
