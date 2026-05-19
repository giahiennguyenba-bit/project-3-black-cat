# 🎨 Vai trò 3: Người Làm Asset & Animation (Animation Artist)

> **Nhiệm vụ:** Tạo hoặc tìm các sprite sheet cho nhân vật, quái vật, boss, và hiệu ứng phép thuật.

---

## 🧠 Sprite Sheet là gì?
Sprite sheet là **một file ảnh PNG duy nhất** chứa nhiều khung hình (frame) xếp ngang hàng. Game sẽ "cắt" từng frame để tạo hiệu ứng chuyển động.

```
Ví dụ file IDLE.png (mèo đứng yên, 10 frame):
┌──────┬──────┬──────┬──────┬──────┐ ...
│ F1   │ F2   │ F3   │ F4   │ F5   │
│ 🐱   │ 🐱   │ 🐱   │ 🐱   │ 🐱   │
└──────┴──────┴──────┴──────┴──────┘
  64px   16px  64px (mỗi frame rộng 64, cách nhau 16px)
```

---

## 📏 Thông số kỹ thuật BẮT BUỘC

| Thuộc tính | Giá trị |
| :--- | :--- |
| Kích thước mỗi frame | `64 x 64 px` |
| Khoảng cách giữa frame (spacing) | `16 px` |
| Hướng nhân vật mặc định | Quay mặt sang **TRÁI** |
| Nền ảnh | **Trong suốt** (transparent PNG) |
| Xếp frame | Ngang — tất cả trên **1 hàng** duy nhất |

---

## 📂 Animation hiện có

Thư mục: `FREE_Cat 2D Pixel Art/.../Sprites/`

| File | Animation | Số frame |
| :--- | :--- | :--- |
| `IDLE.png` | Mèo đứng yên | 10 |
| `WALK.png` | Mèo đi bộ | 12 |
| `RUN.png` | Mèo chạy | 8 |
| `JUMP.png` | Mèo nhảy | 3 |
| `ATTACK 1.png` | Mèo tấn công | 8 |
| `HURT.png` | Mèo bị thương | (chưa dùng) |

Hiệu ứng phép thuật: `Arcane_Effect/01` đến `06` — mỗi thư mục có ~7 frame ảnh riêng lẻ.

---

## ✏️ Nhiệm vụ cụ thể

### 1. Sprite sheet cho Quái vật (Enemy)
Mỗi loại quái cần ít nhất: `IDLE`, `WALK/RUN`, `ATTACK`, `HURT` (tùy chọn), `DEATH` (tùy chọn).

### 2. Sprite sheet cho Boss
Boss cần: `IDLE`, `ATTACK_1`, `ATTACK_2`, `SKILL`, `HURT`, `DEATH`.

### 3. Hiệu ứng kỹ năng (Skill Effects)
Phép thuật, lửa, tia sét... Có thể dùng sẵn `Arcane_Effect/` hoặc tìm thêm.

---

## 🔧 Công cụ miễn phí

| Công cụ | Link |
| :--- | :--- |
| **Piskel** (vẽ online) | https://www.piskelapp.com/ |
| **LibreSprite** (thay thế Aseprite) | https://libresprite.github.io/ |
| **itch.io** (tìm asset miễn phí) | https://itch.io/game-assets/free/tag-pixel-art |
| **OpenGameArt** | https://opengameart.org/ |

---

## 📤 Cách nộp sản phẩm

### Quy tắc đặt tên
```
<Tên-nhân-vật>_<Tên-animation>.png
Ví dụ: Slime_WALK.png, Boss_Dragon_ATTACK_2.png, Effect_Fire_Ball.png
```

### Nơi lưu file
- Sprite nhân vật/quái: tạo thư mục `assets/sprites/<tên>/`
- Hiệu ứng: tạo thư mục `assets/effects/<tên>/`

### Thông tin kèm theo khi nộp
```
Tên file: Slime_WALK.png
Frame size: 64x64 px | Spacing: 16 px | Số frame: 8 | Hướng: Trái
```

---

## ✅ Checklist trước khi nộp
- [ ] PNG nền trong suốt
- [ ] Mỗi frame đúng `64 x 64 px`, spacing `16 px`
- [ ] Nhân vật quay mặt sang **trái**
- [ ] Frame nằm trên 1 hàng ngang
- [ ] Đặt tên đúng quy tắc, ghi rõ số frame cho Nhóm trưởng
