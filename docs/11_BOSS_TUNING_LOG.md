# 📝 Nhật Ký Cân Bằng và Điều Chỉnh Boss (Boss Tuning Log)

Tài liệu này ghi lại chi tiết tất cả các thay đổi và cân bằng đã thực hiện cho hệ thống Boss Battle (`src/boss.c`, `src/boss.h`) để các thành viên khác trong nhóm (đặc biệt là Nhóm trưởng/Implementer) nắm bắt và kiểm tra.

---

## ⚖️ 1. Cân Bằng Chỉ Số Cơ Bản của Boss

### ⏱️ Tốc Độ Hồi Chiêu Chém Cận Chiến (Melee Attack Cooldown)
* **Phase 1:** Thiết lập cooldown là **1.3 giây** (giảm 15% so với ban đầu).
* **Phase 2:** Thiết lập cooldown là **0.875 giây** (giảm 25% so với ban đầu).
* *Mục đích:* Tạo nhịp độ tấn công dồn dập, nguy hiểm hơn khi Boss bước sang Phase 2.

### 🏃 Tốc Độ Di Chuyển (Movement Speed)
* **Tốc độ đi bộ cơ bản (Walk Speed):** Thống nhất ở mức **185.0f** (giảm nhẹ so với ban đầu để người chơi dễ phản xạ).
* **Trạng thái Cuồng Nộ (Rage Mode):** Khi kích hoạt Rage, tốc độ di chuyển tăng thêm **15%** (đạt ~212.75f).

### 🎯 Khoảng Cách Quyết Định Cast Phép (Cast Distance Threshold)
* **Phase 1:** Khoảng cách để Boss bắt đầu cast phép là **125.0f** (giảm một nửa so với 250.0f ban đầu).
* **Phase 2:** Khoảng cách để Boss bắt đầu cast phép là **110.0f** (chỉ thấp hơn Phase 1 một chút, thay vì 83.33f như bản thử nghiệm cũ).
* *Mục đích:* Tránh việc Boss spam phép dồn dập ở cự ly gần, tạo khoảng trống khoảng 110px - 130px để người chơi áp sát cận chiến mà không bị ép góc.

---

## 🔮 2. Điều Chỉnh và Tối Ưu Hóa Chiêu Thức

### ⚡ Chiêu Spark (Ma Thuật Hộ Thân - Aura Sparks)
* **Thời điểm kích hoạt:** Chỉ tự động kích hoạt **1 lần duy nhất** ngay khi Boss bước vào Phase 2 (máu < 50.0f).
* **Thời gian tồn tại:** Kéo dài **5.0 giây**, sau đó tự động tắt và không bao giờ dùng lại.
* **Hoạt ảnh (Animation):** Thay thế toàn bộ hoạt ảnh của quả cầu xung quanh Boss từ `DarkBolt` thành hoạt ảnh `spark` (`assets/spell_png/SSPELL-PNG/spark`) theo yêu cầu thẩm mỹ.
* **Số lượng đạn:** Giới hạn tối đa **2 quả** đạn Spark bám đuổi xuất hiện cùng một lúc trên màn hình.
* **Tốc độ đạn:**
  - Tốc độ xuất hiện ban đầu: Giảm xuống **80.0f** (cũ là 120.0f).
  - Tốc độ bám đuổi (Homing speed): Giảm xuống **80.0f** (cũ là 180.0f).
  - *Mục đích:* Giúp đạn bay chậm, mượt, người chơi dễ dàng né tránh.
* **Dọn dẹp:** Tự động hủy toàn bộ đạn Spark ngay khi Boss chết để đảm bảo công bằng.

### 💣 Chiêu FireBomb
* **Cơ chế:** Gỡ bỏ combo kết hợp (cast DarkBolt rồi bồi thêm FireBomb), chuyển về chiêu **FireBomb thuần** để tránh quá tải hiệu ứng.
* **Độ to (Scale):**
  - **Phase 1:** Tăng kích thước vụ nổ và hitbox thêm **15%** (bán kính tăng lên 57.5f).
  - **Phase 2:** Tăng kích thước vụ nổ và hitbox thêm **20%** (bán kính tăng lên 60.0f).

### 🟣 Chiêu DarkBolt (Quả cầu hắc ám)
* **Tốc độ di chuyển:** Tăng tốc độ bay của quả cầu chính lên **700.0f** (cũ là 450.0f), tạo ra đòn đánh nhanh chớp nhoáng từ xa.

### 🌌 Chiêu DeathSpell (Sét tím tối - SPELL)
* **Tỷ lệ xuất hiện (Trigger Probability):**
  - **Phase 1:** Tăng từ 15% lên **30%** (FireBomb: 30%, DarkBolt: 20%, Lightning: 20%, DeathSpell: 30%).
  - **Phase 2:** Tăng từ 25% lên **40%** (FireBomb: 20%, DarkBolt: 20%, Lightning: 20%, DeathSpell: 40%).

---

## 🛠️ 3. Danh Sách File Đã Thay Đổi
* [boss.c](file:///e:/Project%203/project-3-black-cat/src/boss.c): Code logic AI, cân bằng thông số, cập nhật chiêu thức và tính toán kích thước.
* [boss.h](file:///e:/Project%203/project-3-black-cat/src/boss.h): Khai báo các trường mới phục vụ logic kích hoạt Aura 1 lần (`hasTriggeredPhase2Aura`).
