# SOP Tích Hợp Bản Đồ và Thiết Kế Đấu Trường Boss (SOP Map & Boss Fight Integration)

Tài liệu này cung cấp các hướng dẫn kỹ thuật chi tiết dành cho **Map Maker / Level Designer** để thiết kế bản đồ sao cho tương thích hoàn toàn với hệ thống chiến đấu Boss (Boss Fight) hiện tại sau khi khôi phục camera truyền thống.

---

## 1. Ý Đồ Thiết Kế & Trải Nghiệm Người Chơi (Core Intentions)

* **Thiết kế Camera truyền thống:** Camera sẽ bám sát nhân vật mèo với tỷ lệ zoom cố định **`1.2f`** (không phóng quá to như Dead Cells, giúp người chơi dễ dàng bao quát trận đấu). Camera di chuyển mượt mà theo Cat ở tâm màn hình.
* **Tầm nhìn chiều dọc (Vertical Space):** Mèo luôn đứng ở phần dưới của màn hình (khoảng 72% chiều cao màn hình) để giữ khoảng không phía trên bầu trời lớn, giúp người chơi dễ dàng phản xạ né các đòn tấn công từ trên cao của Boss (như Sét đánh hay Mưa thiên thạch).
* **Chuỗi chuyển cảnh (Intro Walk-in) đơn giản:** Khi chuyển màn, mèo tự động chạy từ ngoài rìa trái (`X = -50.0f`) vào đấu trường (`X = 350.0f`). Camera sẽ trượt theo sát mèo một cách tự nhiên. Không tự động lia camera rời khỏi mèo sang phía Boss.
* **Giới hạn biên camera (Camera Bounds):** 
  - Trong quá trình Intro: Giới hạn biên camera mở rộng ra **`2500.0f`** để camera trượt theo mèo mượt mà mà không bị kẹt ở biên trái.
  - Trong trận đấu (Fight Mode): Giới hạn biên camera co lại còn **`1351.0f`** để khóa camera không cho di chuyển ra ngoài phạm vi đấu trường chính.

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
| **Tỷ lệ thu phóng (Camera Zoom)** | `1.2f` | Hệ số thu phóng cố định của camera trong suốt màn đấu Boss. |

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

1. **Khởi đầu & Giới thiệu (Intro Walk-in / Show Name):** 
   - Mèo xuất hiện ở `X = -50.0f` chạy sang phải.
   - Camera được cấu hình với giới hạn biên rộng `2500.0f` và mức zoom `1.2f`, bám sát Cat.
   - Khi Cat dừng ở `X = 350.0f`, tiếng sấm vang lên và camera rung lắc nhẹ (`CameraShake`). Banner giới thiệu Boss xuất hiện. Camera vẫn giữ tiêu điểm ở Cat.
2. **Chiến đấu (Fight Mode):** 
   - Quyền điều khiển được trả lại cho người chơi.
   - Giới hạn biên camera co về `1351.0f` để giữ khung nhìn hoàn toàn trong đấu trường chính.
   - Camera tiếp tục bám theo Cat mượt mà ở mức zoom `1.2f`.
