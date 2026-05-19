# ⚔️ Vai trò 4: Người Tạo Màn Đánh Boss (Boss Battle Developer)

> **Nhiệm vụ:** Thiết kế hệ thống đánh boss bao gồm: thanh máu (HP), hitbox (vùng va chạm), AI hành vi boss, và hiệu ứng chiêu thức. Bạn **KHÔNG cần file map** — bạn chỉ cần file animation (sprite sheet) từ Animation Artist.

---

## 🧠 Cách tiếp cận

### Ý tưởng chính
Bạn sẽ làm việc **độc lập** với người tạo map. Cụ thể:
1. Nhận sprite sheet boss + hiệu ứng chiêu thức từ **Animation Artist** (vai trò 3).
2. Code hệ thống boss: HP, AI di chuyển, pattern tấn công, hitbox.
3. Với chiêu thức đặc biệt (skill), tạm thời dùng **hình tròn/hình chữ nhật màu** để tượng trưng cho đạn, tia laser, quả cầu lửa...
4. Sau đó, **Nhóm trưởng (Implementer)** sẽ thay thế các hình placeholder đó bằng animation thật và gắn vào game chính.

### Tại sao làm vậy?
- Bạn có thể **test logic boss ngay lập tức** mà không cần chờ map xong.
- Khi gắn vào game chính, chỉ cần thay `DrawCircle()` bằng `DrawTexturePro()` với sprite thật.

---

## 📋 Những gì bạn cần code

### 1. Struct Boss (Cấu trúc dữ liệu Boss)
```c
// boss.h
typedef struct {
    Vector2 position;       // Vị trí boss trên màn hình
    Vector2 velocity;       // Tốc độ di chuyển
    float   maxHP;          // Máu tối đa
    float   currentHP;      // Máu hiện tại
    bool    isAlive;         // Boss còn sống không?
    bool    isAttacking;     // Đang trong trạng thái tấn công?
    int     currentPhase;    // Giai đoạn (boss đổi chiêu khi máu giảm)
    int     attackPattern;   // Kiểu tấn công hiện tại
    float   attackTimer;     // Bộ đếm thời gian giữa các đợt tấn công
    float   attackCooldown;  // Thời gian chờ giữa 2 chiêu
    // Animation
    int     currentFrame;
    float   frameTimer;
    bool    facingRight;
} Boss;
```

### 2. Hệ thống Máu (HP System)
```c
// Vẽ thanh máu boss (thanh đỏ trên đầu boss)
void DrawBossHP(Boss *boss) {
    float barWidth = 200.0f;
    float barHeight = 10.0f;
    float x = boss->position.x - barWidth / 2;
    float y = boss->position.y - 50.0f;  // Phía trên đầu boss

    float hpRatio = boss->currentHP / boss->maxHP;

    // Nền xám
    DrawRectangle((int)x, (int)y, (int)barWidth, (int)barHeight, DARKGRAY);
    // Thanh máu đỏ
    DrawRectangle((int)x, (int)y, (int)(barWidth * hpRatio), (int)barHeight, RED);
    // Viền
    DrawRectangleLines((int)x, (int)y, (int)barWidth, (int)barHeight, WHITE);
}
```

### 3. Hitbox (Vùng va chạm)
```c
// Kiểm tra va chạm giữa Player và Boss
bool CheckHitbox(Rectangle playerAttackBox, Rectangle bossHurtBox) {
    return CheckCollisionRecs(playerAttackBox, bossHurtBox);
}

// Tạo hitbox tấn công cho Player (vùng cào của mèo)
Rectangle GetPlayerAttackBox(Player *player) {
    float attackRange = 40.0f;
    float x = player->facingRight
        ? player->position.x + 20.0f
        : player->position.x - 20.0f - attackRange;
    return (Rectangle){ x, player->position.y - 30.0f, attackRange, 40.0f };
}

// Hitbox thân boss (vùng có thể bị đánh trúng)
Rectangle GetBossHurtBox(Boss *boss) {
    return (Rectangle){
        boss->position.x - 40.0f,
        boss->position.y - 80.0f,
        80.0f, 80.0f
    };
}
```

### 4. Chiêu thức Boss (Dùng hình placeholder tạm)
```c
// Quả cầu lửa — tạm thời vẽ bằng hình tròn màu
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float   radius;
    bool    active;
    float   damage;
} Projectile;

void UpdateProjectile(Projectile *p, float dt) {
    if (!p->active) return;
    p->position.x += p->velocity.x * dt;
    p->position.y += p->velocity.y * dt;
}

void DrawProjectile(Projectile *p) {
    if (!p->active) return;
    // ⬇️ Placeholder: vẽ hình tròn đỏ cam tượng trưng quả cầu lửa
    // Sau này Implementer sẽ thay bằng DrawTexturePro() với sprite thật
    DrawCircleV(p->position, p->radius, (Color){255, 100, 30, 200});
    DrawCircleV(p->position, p->radius * 0.6f, (Color){255, 200, 50, 220});
}
```

### 5. AI Boss đơn giản (Pattern tấn công)
```c
void UpdateBoss(Boss *boss, Player *player, float dt) {
    if (!boss->isAlive) return;

    boss->attackTimer += dt;

    // Boss luôn quay mặt về phía Player
    boss->facingRight = (player->position.x > boss->position.x);

    // Khi hết cooldown → tấn công
    if (boss->attackTimer >= boss->attackCooldown) {
        boss->attackTimer = 0;
        boss->isAttacking = true;

        // Chọn chiêu dựa trên máu
        if (boss->currentHP > boss->maxHP * 0.5f) {
            boss->attackPattern = 0;  // Phase 1: Chiêu cơ bản
        } else {
            boss->attackPattern = 1;  // Phase 2: Chiêu mạnh hơn
        }
    }
}
```

---

## 🧪 Cách Test độc lập (không cần map)

Tạo một file `boss_test.c` riêng để test:
```c
// boss_test.c — Chạy riêng để test logic boss
#include "raylib.h"
// #include "boss.h"  // file bạn tự tạo

int main(void) {
    InitWindow(800, 600, "Boss Battle Test");
    SetTargetFPS(60);

    // Tạo boss ở giữa màn hình
    // Boss boss = InitBoss((Vector2){400, 400}, 100.0f);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        // UpdateBoss(&boss, ..., dt);

        BeginDrawing();
        ClearBackground((Color){30, 30, 50, 255});
        // DrawBoss(&boss);
        // DrawBossHP(&boss);
        DrawText("Boss Battle Test - Press ESC to exit", 10, 10, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
```

Biên dịch riêng:
```bash
gcc boss_test.c -o boss_test.exe -Iraylib/include -Lraylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
```

---

## 📤 Cách nộp sản phẩm
Tạo 2 file: `boss.h` và `boss.c`, đặt trong thư mục `src/`. Báo Nhóm trưởng biết để merge.

## ✅ Checklist
- [ ] Struct Boss có đầy đủ: HP, position, trạng thái, animation frame
- [ ] Thanh máu boss hiển thị đúng
- [ ] Hitbox hoạt động (dùng `CheckCollisionRecs` của Raylib)
- [ ] Boss có ít nhất 2 pattern tấn công
- [ ] Chiêu thức dùng placeholder (hình tròn/chữ nhật) — Implementer sẽ thay sprite sau
- [ ] Đã test riêng bằng file `boss_test.c`
