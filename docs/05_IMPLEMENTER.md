# 🔧 Vai trò 5: Implementer (Người Tích hợp & Gắn kết)

> **Nhiệm vụ:** Nhận tất cả sản phẩm từ các thành viên khác (map, animation, boss) và gắn chúng lại thành **một game hoàn chỉnh** chạy được. Bạn là người "nối dây" cuối cùng.

---

## 🧠 Tổng quan công việc

Bạn sẽ chủ yếu làm việc với các file trong thư mục `src/`:

| File | Vai trò | Bạn cần làm gì |
| :--- | :--- | :--- |
| `main.c` | Điều khiển trung tâm | Thêm map mới, load texture mới, gắn boss vào vòng lặp game |
| `game.c` | Logic nhân vật | Thêm trạng thái mới (bị thương, chết), kết nối hitbox với boss |
| `map.c` | Hệ thống bản đồ | Ít sửa — chỉ cần đảm bảo map mới load đúng |
| `camera.c` | Camera | Ít sửa — điều chỉnh zoom/bounds nếu map mới có kích thước khác |
| `boss.c` (mới) | Logic boss | Nhận từ Boss Dev, sửa lại cho khớp kiến trúc game chính |

---

## 📋 Quy trình tích hợp từng bước

### Bước 1: Tích hợp Map mới

Khi nhận file `.tmj` mới từ người tạo map:

```c
// main.c — Thêm map vào danh sách
const char* mapFiles[] = {
    "assets/back1.tmj",
    "assets/back.tmj",
    "assets/level_2_cave.tmj",    // ← Map mới
    "assets/boss_arena.tmj"       // ← Map đánh boss
};
const int totalMaps = 4;          // ← Cập nhật số lượng
```

Test: `mingw32-make run` → Bấm `R` để chuyển map → Kiểm tra có hiển thị đúng không.

### Bước 2: Tích hợp Animation mới

Khi nhận sprite sheet mới từ Animation Artist:

```c
// main.c — Load texture mới
Texture2D texEnemyIdle = LoadTexture("assets/sprites/slime/Slime_IDLE.png");
Texture2D texEnemyWalk = LoadTexture("assets/sprites/slime/Slime_WALK.png");

// Nhớ giải phóng khi thoát game:
UnloadTexture(texEnemyIdle);
UnloadTexture(texEnemyWalk);
```

Kiểm tra thông số từ Animation Artist:
- Frame size đúng `64x64` chưa?
- Spacing đúng `16px` chưa?
- Số frame đúng chưa?

### Bước 3: Tích hợp Boss Battle

Khi nhận `boss.h` + `boss.c` từ Boss Dev:

1. **Copy file** vào `src/`
2. **Cập nhật Makefile:**
```makefile
SRC = src/main.c src/game.c src/camera.c src/map.c src/boss.c
```
3. **Include trong main.c:**
```c
#include "boss.h"
```
4. **Gắn boss vào vòng lặp game:**
```c
// Trong main(), sau khi khởi tạo Player:
Boss boss = InitBoss((Vector2){600, groundY}, 100.0f);

// Trong vòng lặp while:
UpdateBoss(&boss, &player, dt);

// Trong phần vẽ (giữa BeginMode2D và EndMode2D):
DrawBoss(&boss, texBossIdle, texBossAttack, 64, 64, 1.0f);
DrawBossHP(&boss);
```

### Bước 4: Thay Placeholder bằng Sprite thật

Tìm các `DrawCircle()` / `DrawRectangle()` trong `boss.c` mà Boss Dev dùng làm placeholder → thay bằng `DrawTexturePro()` với sprite thật:

```c
// TRƯỚC (placeholder):
DrawCircleV(p->position, p->radius, (Color){255, 100, 30, 200});

// SAU (sprite thật):
Rectangle src = { frameIdx * 64.0f, 0, 64.0f, 64.0f };
Rectangle dst = { p->position.x, p->position.y, 64.0f, 64.0f };
DrawTexturePro(texFireball, src, dst, (Vector2){32, 32}, 0, WHITE);
```

### Bước 5: Kết nối Hitbox

Đảm bảo Player có thể gây sát thương lên Boss và ngược lại:

```c
// Trong vòng lặp game:
if (player.isAttacking) {
    Rectangle playerHit = GetPlayerAttackBox(&player);
    Rectangle bossHurt  = GetBossHurtBox(&boss);
    if (CheckCollisionRecs(playerHit, bossHurt)) {
        boss.currentHP -= 10.0f;  // Player gây 10 dmg
        if (boss.currentHP <= 0) boss.isAlive = false;
    }
}
```

---

## ⚠️ Lưu ý quan trọng

1. **Luôn gọi `MapUnload()` trước khi load map mới** — tránh rò rỉ bộ nhớ.
2. **Giải phóng tất cả texture** khi thoát game bằng `UnloadTexture()`.
3. **Test sau mỗi bước tích hợp** — đừng gắn hết rồi mới chạy thử.
4. **Giữ nhánh riêng** `feature/integrate-...` — chỉ merge vào `main` khi mọi thứ chạy ổn.

---

## ✅ Checklist tích hợp cuối cùng
- [ ] Tất cả map mới hiển thị đúng khi bấm `R`
- [ ] Nhân vật đứng đúng vị trí mặt đất trên mọi map
- [ ] Animation quái/boss hiển thị đúng frame, đúng hướng
- [ ] Hitbox hoạt động: Player đánh trúng Boss, Boss đánh trúng Player
- [ ] Thanh máu Boss hiển thị đúng
- [ ] Chiêu thức Boss dùng sprite thật (không còn placeholder)
- [ ] Camera hoạt động mượt mà trên mọi map
- [ ] Game không crash, không rò rỉ bộ nhớ
- [ ] Code đã push lên nhánh `main` trên GitHub
