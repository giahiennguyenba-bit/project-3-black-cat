# 🤖 Hướng dẫn giao tiếp với AI Agent (Tích hợp Code)

> **Mục đích:** File này hướng dẫn Implementer (hoặc bất kỳ ai cầm code chính) cách trò chuyện (prompt) với AI Agent để AI hiểu chính xác yêu cầu và tự động viết code đè/tích hợp tài nguyên mới vào game mà không gây lỗi.

Nguyên tắc chung khi nhờ AI code: **Càng cụ thể về tên file, đường dẫn và thông số thì AI code càng chuẩn và chạy được ngay.**

---

## 🗺️ 1. Tích hợp / Thay thế Map mới (.tmj)

Khi bạn nhận được file `.tmj` từ người làm Map, hãy đảm bảo file đó đã được copy vào thư mục `assets/` (hoặc thư mục tương tự). 

**✅ Câu lệnh (Prompt) chuẩn:**
> *"AI ơi, team tôi vừa tạo một map mới tên là `assets/forest_level2.tmj`. Cậu hãy cập nhật file `main.c` để thêm map này vào danh sách chuyển map (`mapFiles`). Hãy tự đọc lại file `.tmj` này nếu cần để kiểm tra logic mặt đất (layer 'ground')."*

**❌ Câu lệnh kém:**
> *"Thêm map mới vào game cho tôi."* (AI không biết tên map là gì, để ở đâu).

---

## 🐱 2. Tích hợp / Đè ngược Sprite Nhân vật chính (Mèo)

Khi Animation Artist giao file sprite sheet mới, AI cần biết **chính xác** kích thước từng khung hình (frame) để viết code tính toán việc cắt ảnh.

**✅ Câu lệnh (Prompt) chuẩn:**
> *"Tôi muốn đè ngược sprite sheet mới cho con mèo. 
> 1. Các file hình nằm ở: `assets/sprites/new_cat/`. Tên file là `IDLE.png`, `RUN.png`, `ATTACK.png`.
> 2. Kích thước mỗi frame: Rộng 64px, Cao 64px. Khoảng cách (spacing) giữa các frame là 16px.
> 3. File IDLE có 8 frame, RUN có 10 frame, ATTACK có 6 frame.
> 
> Cậu hãy vào `main.c` đổi đường dẫn load ảnh, và vào `game.c` cập nhật lại logic cắt frame trong hàm `DrawPlayer` và `UpdatePlayer` cho khớp với thông số trên."*

**❌ Câu lệnh kém:**
> *"Con mèo đổi hình rồi, cậu sửa code cho nó chạy đúng đi."* (AI không biết số lượng frame hay spacing, sẽ phải tự mò hoặc đoán sai làm hình ảnh hiển thị bị cắt lỗi).

---

## ⚔️ 3. Tích hợp Boss và Kỹ năng (Boss Battle)

Đây là phần phức tạp nhất. Boss Dev sẽ code logic bằng các hình vẽ tạm (placeholder), sau đó Artist sẽ vẽ sprite thật. Việc của bạn là nhờ AI gắn kết chúng lại.

**✅ Câu lệnh (Prompt) chuẩn:**
> *"Boss Dev đã viết xong file `src/boss.c` và `src/boss.h`. 
> 1. Cậu hãy cập nhật `Makefile` để compile thêm `boss.c`.
> 2. Cập nhật `main.c` để khởi tạo Boss ở vị trí (X: 800, Y: bằng với mặt đất) và đưa boss vào vòng lặp game.
> 3. Artist cũng đã vẽ xong hình Boss. File nằm ở `assets/boss/boss_idle.png` (8 frame, 128x128px, không có spacing) và `assets/effects/fireball.png` (4 frame, 64x64px).
> 
> Cậu hãy sửa lại `boss.c`, xóa các lệnh vẽ hình tròn/hình chữ nhật tạm đi, thay bằng lệnh `DrawTexturePro` để load và chạy animation cho các sprite thật. Cuối cùng, thêm logic xử lý va chạm: khi mèo chém trúng boss thì trừ 10 máu."*

**❌ Câu lệnh kém:**
> *"Gắn con boss vào game và thay hình cho nó."* (AI không biết tên hàm trong `boss.c`, không biết kích thước hình ảnh boss để canh chỉnh).

---

## 🛠️ Tổng kết: Công thức Vàng khi ra lệnh cho AI

Để làm việc chuẩn chỉ với AI, hãy luôn cung cấp đủ 3 yếu tố sau trong câu lệnh:
1. **Hành động:** Bạn muốn tôi làm gì? (thêm map, sửa code vẽ, thêm hitbox...)
2. **Vị trí (File path):** File mới nằm ở đâu? Cần sửa file C nào?
3. **Thông số kỹ thuật:** Kích thước px, số lượng frame, tên layer...

Nếu bạn tuân thủ công thức này, AI sẽ tích hợp mọi thứ một cách hoàn hảo chỉ trong vài giây!
