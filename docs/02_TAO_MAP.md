# 🗺️ Vai trò 2: Người Tạo Map (Tiled Map Designer)

> **Nhiệm vụ:** Thiết kế các bản đồ (level) cho game bằng ứng dụng **Tiled Map Editor**, export ra file `.tmj` để code có thể đọc được.

---

## 🛠️ Cài đặt công cụ

### Bước 1: Tải Tiled Map Editor (MIỄN PHÍ)
- Vào trang: https://www.mapeditor.org/
- Tải phiên bản cho Windows → Cài đặt bình thường.
- Hoặc tải trên **itch.io** (miễn phí): https://thorbjorn.itch.io/tiled

### Bước 2: Mở thử file map hiện có
- Mở Tiled → `File > Open` → chọn file `assets/back1.tmj` trong dự án.
- Xem cách nhóm gốc đã thiết kế map để hiểu cấu trúc.

---

## 📐 Quy tắc bắt buộc khi tạo map

### 1. Kích thước Tile
- **Tile Width:** `16px`
- **Tile Height:** `16px`
- Đây là kích thước chuẩn mà code đang sử dụng. **KHÔNG ĐƯỢC thay đổi.**

### 2. Tileset (Bộ gạch hình ảnh)
- Tileset chính hiện tại: `LAMO/Final/Tiles.png`
- Khi tạo map mới, hãy **import tileset này** vào Tiled:
  - `Map > New Tileset > Browse > chọn file Tiles.png`
  - Đặt Tile Width = 16, Tile Height = 16
- Bạn CÓ THỂ thêm tileset mới (ví dụ: thêm hình cây, đá, hang động...), code đã hỗ trợ **nhiều tileset trên cùng 1 map**.

### 3. Layer bắt buộc: `ground`
> ⚠️ **ĐÂY LÀ QUY TẮC QUAN TRỌNG NHẤT**

Code dùng layer có tên `"ground"` để xác định vị trí mặt đất cho nhân vật đứng.

**Cách tạo:**
1. Trong Tiled, tạo một **Object Layer** (không phải Tile Layer).
2. Đặt tên chính xác là: **`ground`** (viết thường, không có dấu cách, không viết hoa).
3. Vẽ **một hình chữ nhật (Rectangle)** tại vị trí mà bạn muốn mặt đất nằm.
4. Tọa độ Y (trục dọc) của hình chữ nhật này sẽ là nơi nhân vật mèo đứng.

```
Ví dụ minh họa:
┌─────────────────────────────┐
│        Bầu trời             │
│                             │
│     🌳      🌳      🌳       │
│                             │
│▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓│ ← Đây là vị trí Y của Object trong layer "ground"
│█████████████████████████████│    Mèo sẽ đứng ở đây
│█████████████████████████████│
└─────────────────────────────┘
```

### 4. Các Layer khác (tùy ý)
Bạn có thể tạo nhiều Tile Layer để trang trí map. Gợi ý:
- `background` — hình nền xa (bầu trời, núi)
- `midground` — cây cối, bụi rậm ở giữa
- `foreground` — vật thể ở gần camera nhất (cành cây che phủ)
- `decoration` — hoa, cỏ, đá nhỏ

> 💡 **Mẹo:** Các layer được vẽ **theo thứ tự từ dưới lên trên** trong Tiled. Layer ở dưới cùng sẽ được vẽ trước (nằm phía sau trong game).

---

## 📤 Cách Export Map

### Bắt buộc export ra định dạng `.tmj` (JSON)
1. Hoàn thành thiết kế map.
2. Vào `File > Export As...`
3. Chọn định dạng: **JSON map files (*.tmj)** hoặc **(*.json)**
4. Lưu file vào thư mục `assets/` của dự án. Ví dụ: `assets/level_2_cave.tmj`

### Kiểm tra sau khi export
Mở file `.tmj` bằng Notepad hoặc VS Code, đảm bảo:
- Có `"layers"` chứa layer tên `"ground"` với `"type": "objectgroup"`
- Đường dẫn ảnh tileset (trường `"image"`) trỏ đúng vị trí file ảnh

---

## 🔗 Cách thêm map mới vào game (báo Nhóm trưởng làm)
Sau khi bạn tạo xong file `.tmj`, hãy báo Nhóm trưởng. Họ sẽ thêm vào `main.c`:

```c
// Trong main.c, thêm đường dẫn map mới vào mảng:
const char* mapFiles[] = {
    "assets/back1.tmj",
    "assets/back.tmj",
    "assets/level_2_cave.tmj"   // ← Map mới của bạn
};
const int totalMaps = 3;        // ← Cập nhật số lượng map
```

---

## ✅ Checklist trước khi nộp map

- [ ] File tileset (`.png`) nằm đúng thư mục và đường dẫn khớp trong file `.tmj`
- [ ] Có layer tên `ground` (Object Layer) với ít nhất 1 Rectangle Object
- [ ] Export đúng định dạng `.tmj` (JSON)
- [ ] File `.tmj` nằm trong thư mục `assets/`
- [ ] Đã test mở lại file `.tmj` trong Tiled mà không bị lỗi
- [ ] Thông báo cho Nhóm trưởng đường dẫn file

---

## 💡 Mẹo thiết kế hay
- Bản đồ nên rộng theo chiều ngang (scrolling ngang) để phù hợp với game platformer.
- Thử thay đổi `opacity` (độ trong suốt) của layer background để tạo hiệu ứng chiều sâu.
- Sử dụng Object Layer để đánh dấu vị trí boss, cửa chuyển map, hoặc vật phẩm — Nhóm trưởng sẽ dùng thông tin này khi code.
