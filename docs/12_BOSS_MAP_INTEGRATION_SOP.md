# SOP Tích Hợp Bản Đồ và Thiết Kế Đấu Trường Boss (SOP Map & Boss Fight Integration)

Tài liệu này cung cấp các hướng dẫn kỹ thuật chi tiết dành cho **Map Maker / Level Designer** để thiết kế bản đồ sao cho tương thích hoàn toàn với hệ thống chiến đấu Boss (Boss Fight) hiện tại.

---

## 1. Ý Đồ Thiết Kế & Trải Nghiệm Người Chơi (Core Intentions)

* **Thiết kế Camera kiểu Dead Cells:** Camera sẽ phóng to gần vào nhân vật mèo (Zoom cố định = `1.6f`) để mang lại trải nghiệm hành động chặt chém tốc độ cao và rõ ràng. Camera sẽ tự động cuộn (scroll) theo trục ngang khi mèo di chuyển và tự động khóa biên khi chạm ranh giới đấu trường.
* **Tầm nhìn chiều dọc (Vertical Space):** Mèo luôn đứng ở phía dưới màn hình (khoảng 72% chiều cao màn hình) để người chơi có khoảng không nhìn lên trên bầu trời, giúp phản xạ né tránh các đòn đánh từ trên cao của Boss (như Sét đánh Thiên Lôi Phạt hay Mưa bom lửa FireBomb).
* **Chuỗi chuyển cảnh (Cutscene) mở đầu:** Khi chuyển map, mèo sẽ chạy từ ngoài màn hình bên trái vào đấu trường, camera trượt theo mèo, sau đó lia (pan) sang hiển thị Boss ở bên phải để tạo sự hoành tráng, và cuối cùng lia lại mèo để bắt đầu chiến đấu.

---

## 2. Thông Số Kỹ Thuật Bắt Buộc (Technical Specifications)

Dưới đây là các thông số tọa độ thực tế trong thế giới game (World Coordinates) mà bản đồ cần tuân thủ:

| Thông số | Giá trị tọa độ | Ý nghĩa / Ghi chú |
| :--- | :--- | :--- |
| **Độ cao mặt đất (Ground Y)** | `640.0f` | Toàn bộ bề mặt sàn đấu trường phải phẳng hoàn toàn tại độ cao này. |
| **Giới hạn biên trái (Left Barrier)** | `35.0f` | Vị trí xuất hiện tường năng lượng đỏ khóa người chơi không cho đi sang trái. |
| **Giới hạn biên phải (Right Barrier)** | `1315.0f` | Vị trí xuất hiện tường năng lượng đỏ khóa người chơi không cho đi sang phải. |
| **Điểm dừng chân mèo (Player Walk-in Stop)** | `350.0f` | Mèo sẽ tự chạy từ `-50.0f` bên ngoài vào và dừng lại tại đây trong Cutscene. |
| **Điểm spawn của Boss (Boss Spawn X)** | `950.0f` | Boss đứng yên thủ thế ở vị trí này đối diện người chơi trong Cutscene. |
| **Phạm vi an toàn hình ảnh (Visual Padding)** | `[-1500, 3500]` | Bản vẽ trang trí nền sau và mặt đất cần kéo dài trong khoảng này để tránh hiện viền đen khi camera cuộn. |

---

## 3. Các Yêu Cầu Cụ Thể Cho Bản Đồ Trong Tiled (.TMJ)

### A. Bản đồ Đấu trường Boss (`boss1.tmj`):
* **Địa hình sàn đấu (Terrain):**
  - Mặt đất chính của đấu trường nằm trong khoảng `X` từ `0` đến `1351.0f` phải phẳng hoàn toàn ở `Y = 640.0f`.
  - Phía bên ngoài rào chắn ma thuật (từ `X < 0` về bên trái và `X > 1351` về bên phải) cần tiếp tục vẽ thêm gạch sàn và cảnh nền (đến tối thiểu `X = -1500` và `X = 3500`) để làm đệm hình ảnh khi camera di chuyển theo nhân vật lúc chạy Intro hoặc khi thoát map.
* **Các Layer Va Chạm ("ground" object layer):**
  - Cần vẽ một hình chữ nhật va chạm phẳng trong layer `"ground"` ở độ cao `640.0f` để mèo có thể đứng vững.
  - Tránh đặt các vật thể va chạm tường đứng ở giữa khu vực `[35, 1315]` để không cản trở chuyển động chiến đấu của mèo và Boss.

### B. Bản đồ Hướng dẫn (`tutorial map.tmj`):
* **Điểm chuyển cảnh sang Boss (Exit to Boss):**
  - Khi người chơi chạy hết sang biên trái của map Hướng dẫn (`X < 16.0f`), game sẽ tự động chuyển cảnh sang màn Boss.
  - Hãy thiết kế địa hình biên trái mở (không có tường va chạm) ở độ cao phù hợp để người chơi đi qua một cách tự nhiên.
* **Điểm quay về từ Boss (Exit back from Boss Arena):**
  - Sau khi Boss bị tiêu diệt, rào chắn ma thuật bên phải đấu trường biến mất. Nếu người chơi đi quá rìa phải (`X > 1315.0f`), game sẽ chuyển ngược lại về map Hướng dẫn.
  - Điểm xuất hiện lại (Spawn Point) trên map hướng dẫn khi quay về sẽ là điểm spawn mặc định của map Hướng dẫn (`assets/tutorial map.tmj`).

---

## 4. Tóm Tắt Luồng Chuyển Cảnh Camera (Camera SOP)

1. **Khởi đầu (Intro Walk-in):** Mèo xuất hiện ở `X = -50.0f` chạy sang phải. Camera không bị giới hạn biên, bám sát mèo giữ ở tâm màn hình cho đến khi mèo đứng ở `X = 350.0f`.
2. **Lia sang Boss (Pan to Boss):** Mèo đứng tĩnh, camera trượt mượt mà sang phải hiển thị Boss ở `X = 950.0f` (trong 2.0 giây). Tiếng sấm vang lên và camera rung lắc.
3. **Lia lại Mèo (Pan back to Player):** Camera trượt ngược lại về bên trái hiển thị lại mèo ở `X = 350.0f` (trong 2.8 giây) kèm banner giới thiệu Boss.
4. **Chiến đấu (Fight Mode):** Camera khóa biên đấu trường (`bounds = [0, 1351]`), tự động cuộn mượt theo mèo ở mức zoom **`1.6f`** khi mèo di chuyển trong khu vực đấu.
