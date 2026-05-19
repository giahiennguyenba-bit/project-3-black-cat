# 👑 Vai trò 1: Nhóm Trưởng (Team Leader)

> **Người đảm nhận:** Bạn — người điều phối toàn bộ nhóm và chịu trách nhiệm cuối cùng cho sản phẩm.

---

## 🎯 Nhiệm vụ chính
1. **Điều phối công việc** giữa 4 thành viên còn lại, đảm bảo mọi người hiểu rõ nhiệm vụ của mình.
2. **Quản lý tiến độ** — theo dõi ai đang làm gì, có vướng mắc ở đâu.
3. **Quyết định kỹ thuật** — khi có xung đột code hoặc hai người làm trùng, bạn là người phán quyết.
4. **Tích hợp cuối cùng (Implementation)** — gắn mọi thứ (map, animation, boss, chiêu thức) lại thành một game hoàn chỉnh chạy được.

---

## 📋 Tổng quan dự án (Đọc trước khi bắt đầu)

### Dự án này là gì?
Đây là game platformer 2D viết bằng **ngôn ngữ C** kết hợp thư viện đồ họa **Raylib**. Người chơi điều khiển một chú mèo đen khám phá rừng, chiến đấu với quái vật/boss.

### Công nghệ sử dụng
| Thành phần | Công nghệ |
| :--- | :--- |
| Ngôn ngữ lập trình | C (chuẩn C99) |
| Thư viện đồ họa | Raylib (đã có sẵn trong thư mục `raylib/`) |
| Đọc bản đồ Tiled | `cute_tiled.h` (thư viện header-only, nằm trong `src/`) |
| Tạo bản đồ | Ứng dụng **Tiled Map Editor** (tải miễn phí) |
| Build system | Makefile + GCC (MinGW trên Windows) |
| Quản lý phiên bản | Git + GitHub |

### Cấu trúc thư mục hiện tại
```
project-3-black-cat/
├── src/                          # 🧠 Source code chính
│   ├── main.c                    # Vòng lặp game, khởi tạo, chuyển map
│   ├── game.c / game.h           # Logic nhân vật (di chuyển, nhảy, đánh)
│   ├── map.c / map.h             # Load & vẽ bản đồ Tiled (.tmj)
│   ├── camera.c / camera.h       # Camera bám theo nhân vật, rung màn hình
│   ├── cute_tiled.h              # Thư viện đọc file Tiled (KHÔNG SỬA)
│   └── parson.c / parson.h       # Thư viện parse JSON (KHÔNG SỬA)
├── assets/                       # 🗺️ Bản đồ (.tmj) và hình nền
│   ├── back.tmj, back1.tmj       # File bản đồ Tiled đã export
│   └── player/                   # (dự phòng) hình nhân vật
├── FREE_Cat 2D Pixel Art/        # 🐱 Sprite sheet mèo (IDLE, WALK, RUN, JUMP, ATTACK)
├── Arcane_Effect/                # ✨ Hiệu ứng phép thuật (sprite sheet)
├── LAMO/Final/                   # 🌿 Tileset rừng (Tiles.png, Background...)
├── Free Pixel Art Forest/        # 🌲 Thêm asset rừng pixel art
├── Godot Project V4/             # 📦 Tham khảo từ Godot (NinjaAdventure)
├── raylib/                       # 📚 Thư viện Raylib đã biên dịch sẵn
├── Makefile                      # ⚙️ Lệnh build (mingw32-make run)
└── README.md                     # 📖 Giới thiệu dự án
```

---

## 🔄 Quy trình làm việc nhóm (Git Workflow)

### Nguyên tắc vàng
- **Nhánh `main`** = phiên bản chạy ổn định nhất. **KHÔNG BAO GIỜ** push code chưa test lên đây.
- Mỗi người làm trên **nhánh riêng** của mình.
- Nhóm trưởng (bạn) sẽ **review và merge** code từ các nhánh vào `main`.

### Quy ước đặt tên nhánh
| Vai trò | Tên nhánh |
| :--- | :--- |
| Người tạo Map | `feature/map-<tên-map>` |
| Người làm Animation | `feature/anim-<tên-nhân-vật>` |
| Người làm Boss Battle | `feature/boss-<tên-boss>` |
| Implementer (bạn) | `feature/integrate-<mô-tả>` |

### Các bước merge code (dành cho Nhóm trưởng)
```bash
# 1. Kéo code mới nhất từ GitHub
git pull origin main

# 2. Chuyển sang nhánh của thành viên để kiểm tra
git checkout feature/map-forest-2
git pull origin feature/map-forest-2

# 3. Chạy thử xem có lỗi không
mingw32-make run

# 4. Nếu OK, merge vào main
git checkout main
git merge feature/map-forest-2

# 5. Đẩy lên GitHub
git push origin main
```

---

## 📊 Bảng phân công (Template)
Hãy copy bảng này vào nhóm chat để theo dõi tiến độ:

| # | Thành viên | Vai trò | Nhiệm vụ cụ thể | Deadline | Trạng thái |
| :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | (Tên bạn) | Nhóm trưởng + Implementer | Tích hợp tất cả, sửa bug | ... | 🟡 Đang làm |
| 2 | ... | Tạo Map (Tiled) | Thiết kế 3+ bản đồ rừng | ... | ⬜ Chưa bắt đầu |
| 3 | ... | Animation Artist | Sprite sheet mèo + quái | ... | ⬜ Chưa bắt đầu |
| 4 | ... | Boss Battle Dev | Màn đánh boss + HP + hitbox | ... | ⬜ Chưa bắt đầu |
| 5 | ... | Implementer #2 | Hỗ trợ gắn boss vào game | ... | ⬜ Chưa bắt đầu |

---

## ⚠️ Lưu ý quan trọng cho Nhóm trưởng
1. **Đảm bảo mọi người đọc file hướng dẫn của mình** trước khi bắt tay vào làm.
2. **Kiểm tra code trước khi merge** — chạy `mingw32-make run` và đảm bảo game không crash.
3. **Giao tiếp rõ ràng** — nếu thành viên cần file gì (ví dụ: sprite sheet kích thước bao nhiêu, map bao lớn), hãy thống nhất sớm.
4. **Backup thường xuyên** — sau mỗi buổi làm việc, push code lên GitHub.
