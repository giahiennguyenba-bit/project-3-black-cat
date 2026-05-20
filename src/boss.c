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

    resourcesLoaded = false;
}

void InitBoss(Boss *boss, Vector2 pos) {
    boss->position = pos;
    boss->velocity = (Vector2){0, 0};
    boss->maxHP = 600.0f;
    boss->currentHP = 600.0f;
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
    
    // Tự động load resources nếu chưa có
    LoadBossResources();
}

void UpdateBoss(Boss *boss, Player *player, MyCamera *cam, float dt) {
    if (!boss->isAlive) return;

    // Cập nhật trạng thái HURT / DEATH
    if (boss->currentHP <= 0 && boss->state != BOSS_STATE_DEATH) {
        boss->state = BOSS_STATE_DEATH;
        boss->currentFrame = 0;
        boss->frameTimer = 0.0f;
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
            // Điểm chém trúng: Frame 5-7
            if (boss->currentFrame >= 5 && boss->currentFrame <= 7) {
                float dist = fabsf(boss->position.x - player->position.x);
                float heightDiff = fabsf(boss->position.y - player->position.y);
                if (dist < 110.0f && heightDiff < 50.0f) {
                    // Người chơi bị dính đòn cận chiến!
                    if (player->freezeTimer <= 0) {
                        player->freezeTimer = 0.25f;
                        // Đẩy nhẹ player ra sau
                        float pushDir = (player->position.x > boss->position.x) ? 1.0f : -1.0f;
                        player->position.x += pushDir * 30.0f;
                        CameraShake(cam, 0.3f, 8.0f); // Rung lắc màn hình khi player bị chém
                        TraceLog(LOG_INFO, "Player hit by Boss Melee!");
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
                
                // Ở frame thứ 5, bắn ra DarkBolt
                if (boss->currentFrame == 5 && !boss->projectile.active) {
                    boss->projectile.active = true;
                    boss->projectile.position = (Vector2){ boss->position.x + (boss->facingRight ? 50 : -50), boss->position.y - 45 };
                    float dir = boss->facingRight ? 1.0f : -1.0f;
                    boss->projectile.velocity = (Vector2){ dir * 450.0f, 0.0f };
                    boss->projectile.currentFrame = 0;
                    boss->projectile.frameTimer = 0.0f;
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

        boss->attackTimer += dt;

        if (distAbs > 250.0f) {
            // Player ở quá xa -> Đi lại gần
            boss->state = BOSS_STATE_WALK;
            float moveDir = (distToPlayer > 0) ? 1.0f : -1.0f;
            boss->velocity.x = moveDir * 100.0f;
            boss->position.x += boss->velocity.x * dt;
        } else if (distAbs < 80.0f) {
            // Player ở quá gần -> Lùi lại một chút hoặc chém cận chiến ngay
            if (boss->attackTimer >= boss->attackCooldown) {
                boss->state = BOSS_STATE_ATTACK;
                boss->currentFrame = 0;
                boss->frameTimer = 0;
                boss->attackTimer = 0;
                boss->isAttacking = true;
                boss->velocity.x = 0;
            } else {
                boss->state = BOSS_STATE_IDLE;
                boss->velocity.x = 0;
            }
        } else {
            // Player ở khoảng giữa -> IDLE hoặc dùng CAST phép từ xa
            boss->state = BOSS_STATE_IDLE;
            boss->velocity.x = 0;

            if (boss->attackTimer >= boss->attackCooldown) {
                // Tùy biến đòn đánh theo Phase
                boss->attackTimer = 0;
                if (GetRandomValue(0, 10) > 4) {
                    boss->state = BOSS_STATE_CAST;
                    boss->isCasting = true;
                } else {
                    boss->state = BOSS_STATE_ATTACK;
                    boss->isAttacking = true;
                }
                boss->currentFrame = 0;
                boss->frameTimer = 0;
            }
        }
    }

    // 3. Cập nhật Projectile (DarkBolt phép thuật)
    if (boss->projectile.active) {
        boss->projectile.position.x += boss->projectile.velocity.x * dt;
        boss->projectile.position.y += boss->projectile.velocity.y * dt;
        
        // Hoạt ảnh Projectile
        boss->projectile.frameTimer += dt;
        if (boss->projectile.frameTimer >= 0.08f) {
            boss->projectile.frameTimer = 0.0f;
            boss->projectile.currentFrame = (boss->projectile.currentFrame + 1) % boss->projectile.maxFrames;
        }

        // Tự hủy nếu bay quá xa khỏi tầm nhìn
        if (fabsf(boss->projectile.position.x - boss->position.x) > 1200.0f) {
            boss->projectile.active = false;
        }

        // Kiểm tra va chạm với Player
        float distToPlayer = Vector2Distance(boss->projectile.position, (Vector2){ player->position.x, player->position.y - 32.0f });
        if (distToPlayer < (boss->projectile.radius + 20.0f)) {
            boss->projectile.active = false;
            
            // Làm cho player bị đóng băng/đẩy lùi
            if (player->freezeTimer <= 0) {
                player->freezeTimer = 0.3f;
                float pushDir = (boss->projectile.velocity.x > 0) ? 1.0f : -1.0f;
                player->position.x += pushDir * 40.0f;
                CameraShake(cam, 0.25f, 6.0f); // Rung lắc khi spell bắn trúng player
                TraceLog(LOG_INFO, "Player hit by Boss DarkBolt spell!");
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

        if (distAbs < attackDist && correctDirection && boss->state != BOSS_STATE_HURT && boss->state != BOSS_STATE_DEATH) {
            // Boss bị dính đòn của player!
            boss->currentHP -= 15.0f;
            if (boss->currentHP < 0) boss->currentHP = 0;
            
            boss->state = BOSS_STATE_HURT;
            boss->currentFrame = 0;
            boss->frameTimer = 0.0f;
            boss->isHurt = true;
            
            // Knocks back slightly
            float kb = (player->facingRight) ? -15.0f : 15.0f;
            boss->position.x += kb;
            
            CameraShake(cam, 0.2f, 5.0f); // Rung lắc khi chém trúng Boss
            TraceLog(LOG_INFO, "Boss hit by Player! Current HP: %.1f", boss->currentHP);
        }
    }
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
    float yOffset = 18.0f; 
    Rectangle dest = { 
        boss->position.x, 
        boss->position.y + yOffset, 
        frameW * scale, 
        frameH * scale 
    };

    // Điểm tựa (origin) ở đáy giữa khung hình
    Vector2 origin = { (frameW * scale) / 2.0f, frameH * scale };

    // Vẽ Boss với hiệu ứng chớp đỏ nếu bị đau
    Color tint = WHITE;
    if (boss->state == BOSS_STATE_HURT) {
        tint = (Color){ 255, 120, 120, 255 };
    }

    DrawTexturePro(currentTex, source, dest, origin, 0.0f, tint);

    // Vẽ Projectile nếu đang hoạt động
    if (boss->projectile.active) {
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
