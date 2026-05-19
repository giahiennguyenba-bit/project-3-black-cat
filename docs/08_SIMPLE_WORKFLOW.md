# 🔄 Quy trình làm việc nhóm (Workflow Đơn Giản)

Để dự án trơn tru, mỗi người trong nhóm cần tuân thủ tuyệt đối quy trình dưới đây.

---

## 👥 Các vai trò trong nhóm

1. **Map Maker (Người tạo Map)**: Dùng ứng dụng Tiled để vẽ màn chơi.
2. **Animation Artist (Người làm hình ảnh)**: Vẽ/cắt ghép các sprite sheet cho mèo, quái vật, boss, hiệu ứng.
3. **Boss Battle Dev (Người code Boss)**: Viết AI, thanh máu, và hitbox cho Boss.
4. **Leader & Implementer (Nhóm trưởng - Người Gắn kết)**: Đứng đầu dự án, là người duy nhất được quyền đụng vào nhánh `main` và nối các sản phẩm của 3 người trên lại với nhau.

---

## 🚀 Cách chúng ta làm việc cùng nhau

Toàn bộ quy trình sẽ diễn ra theo 3 bước cực kỳ đơn giản:

### BƯỚC 1: Làm việc độc lập (Các thành viên)
- Khi bắt đầu công việc, bạn (Map Maker, Artist, Boss Dev) tạo một nhánh (branch) riêng trên máy của mình.
- *Ví dụ:* Nhánh `feature/map-rung` hoặc nhánh `feature/hinh-anh-boss`.
- Bạn cứ làm việc bình thường, code hay vẽ trên nhánh này. Sai cũng không sao vì nó không ảnh hưởng đến game gốc.

### BƯỚC 2: Nộp bài (Các thành viên)
- Làm xong, bạn đưa các file của mình (file `.tmj`, hình ảnh `.png`, hoặc file `.c`) lên nhánh riêng của bạn trên GitHub.
- Nhắn tin cho Nhóm trưởng: *"Tôi đã up xong bản đồ lên nhánh `feature/map-rung` rồi, kiểm tra nhé!"*

### BƯỚC 3: Gắn kết và Phát hành (Leader & Implementer)
- Nhóm trưởng sẽ kéo nhánh của bạn về máy.
- Nhóm trưởng tự tay viết code (hoặc nhờ AI) để nối cái map mới hay con boss mới đó vào trong hệ thống game gốc (`main.c`, `Makefile`, v.v.).
- Chạy thử (Test), nếu mượt mà không lỗi thì Nhóm trưởng mới đưa nó lên nhánh `main`.
- Lúc này, mọi người kéo nhánh `main` mới về sẽ thấy bản update hoàn chỉnh!

---

## 🛑 Những "Điều Cấm Kỵ"
1. **Tuyệt đối không đẩy thẳng (push) lên nhánh `main`**: Bạn làm sập nhánh `main` là cả team nghỉ chơi.
2. **Không sửa file của người khác nếu không báo trước**: Ai làm phần người nấy. Nếu bạn làm Map, đừng táy máy vào file code. Nếu bạn làm Code, đừng táy máy vào file hình ảnh trừ khi bạn đã báo với Nhóm trưởng.
3. **Mọi thay đổi lớn phải hỏi Leader**: Vì Leader là người tích hợp, nếu bạn thay đổi cấu trúc file, đổi tên thư mục mà không báo, lúc Leader (Implementer) ghép code sẽ sinh ra 100 cái lỗi ẩn.

---

> 💡 **Tóm lại:** Bạn làm việc ở một góc sân riêng của bạn. Làm xong đóng gói gửi cho Leader. Leader sẽ ráp nó vào bức tranh tổng thể!
