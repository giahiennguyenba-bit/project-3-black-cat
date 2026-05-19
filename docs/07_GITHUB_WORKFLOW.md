# 🐙 Hướng dẫn sử dụng GitHub cho Team "Gà Mờ"

Tài liệu này được viết theo cách dễ hiểu nhất, không dùng từ ngữ học thuật phức tạp. Mục tiêu là giúp cả team **mỗi người tự làm việc riêng của mình** mà không sợ vô tình "phá nát" dự án chung của cả nhóm.

---

## 🌳 Khái niệm cơ bản (Ai cũng phải đọc)

Hãy tưởng tượng toàn bộ dự án game của chúng ta là một **cái cây**:
- **Nhánh `main` (Thân cây chính):** Đây là nơi chứa sản phẩm cuối cùng, ổn định nhất. Quy tắc sống còn: **KHÔNG AI ĐƯỢC CHỈNH SỬA TRỰC TIẾP LÊN NHÁNH `main` NGOẠI TRỪ NHÓM TRƯỞNG.** Nếu nhánh `main` bị lỗi, toàn bộ game sẽ "sập".
- **Nhánh phụ (Branch):** Khi bạn (thành viên) được giao nhiệm vụ, bạn sẽ mọc ra một "cành cây con" từ thân cây chính. Bạn có thể tự do đập phá, sửa file, thêm hình ảnh trên cành cây của bạn. Dù bạn có làm lỗi, cái thân cây gốc (nhánh `main`) vẫn an toàn.
- **Merge (Gộp code):** Khi bạn làm xong nhiệm vụ trên nhánh phụ của mình và chạy ngon lành, Nhóm trưởng sẽ lấy những gì bạn làm và "gắn" nó trở lại vào thân cây chính (`main`). Quá trình nối này gọi là **Merge**.

---

## 🛠️ PHẦN 1: Dành cho CÁC THÀNH VIÊN (Map, Animation, Boss Dev)

Nhiệm vụ của bạn là làm việc trên máy tính của mình, sau đó "gửi" file lên GitHub vào một góc riêng biệt.

### Bước 1: Cập nhật dữ liệu mới nhất trước khi làm việc
Mở Terminal (hoặc Git Bash / VS Code Terminal) và gõ:
```bash
git checkout main
git pull origin main
```
*(Ý nghĩa: Tôi muốn về thân cây chính và kéo tất cả cập nhật mới nhất mà Leader vừa thêm về máy).*

### Bước 2: Tạo nhánh làm việc riêng của bạn
Đừng bao giờ làm việc trên `main`. Hãy tự tạo một nhánh mới có tên nhiệm vụ của bạn:
```bash
git checkout -b feature/tao-map-rung
# Hoặc: git checkout -b feature/ve-boss-rong
# Hoặc: git checkout -b feature/code-boss-ai
```
*(Ý nghĩa: Trích xuất một bản sao từ thân cây chính ra thành cành riêng của tôi).*

### Bước 3: Làm việc bình thường
Bây giờ bạn cứ vẽ map bằng Tiled, vẽ sprite, hoặc code boss. Save file lại trong thư mục dự án bình thường.

### Bước 4: Lưu và "Gửi" bài lên GitHub
Sau khi làm xong (hoặc cuối ngày muốn lưu lại), chạy 3 lệnh sau:
```bash
# 1. Gom tất cả file vừa sửa
git add .

# 2. Đóng gói lại và dán nhãn (ghi chú bạn vừa làm gì)
git commit -m "Đã làm xong map rừng tầng 2"

# 3. Đẩy nhánh riêng của bạn lên GitHub
git push origin feature/tao-map-rung
```
Sau khi gõ lệnh số 3 xong, việc của bạn đã kết thúc! Hãy nhắn tin cho Nhóm trưởng: *"Ê, tui đẩy nhánh `feature/tao-map-rung` lên rồi, ông vào check rồi merge nhé!"*

---

## 👑 PHẦN 2: Dành riêng cho IMPLEMENTER (Nhóm trưởng)

Vì bạn là người giữ repo gốc, bạn có quyền lực tối cao nhưng cũng gánh trách nhiệm giữ cho game không bị "cháy".

### Nhiệm vụ 1: Khóa nhánh `main` (Bảo vệ Repo)
Để phòng hờ đồng đội "quên" và đẩy thẳng code lên `main`, hãy vào trang GitHub của dự án:
1. Vào mục **Settings** > **Branches**.
2. Bấm **Add branch ruleset**.
3. Target branch chọn `main`.
4. Bật tùy chọn **Require a pull request before merging**. 
   *(Như vậy, không ai có thể tự tiện push thẳng lên `main`, bắt buộc phải nộp bài qua nhánh phụ).*

### Nhiệm vụ 2: Kiểm tra bài (Test) của đồng đội
Khi một thành viên báo đã đẩy code lên nhánh `feature/tao-map-rung`, bạn khoan hãy Merge. Hãy đem code đó về máy kiểm tra trước (có thể nhờ AI tích hợp thử).

```bash
# 1. Kéo tất cả nhánh mới từ GitHub về
git fetch origin

# 2. Chui vào nhánh của đồng đội để xem họ làm gì
git checkout feature/tao-map-rung
```
Lúc này, thư mục trên máy bạn sẽ thay đổi theo đúng những gì đồng đội vừa làm. 
Bạn hãy chạy lệnh `mingw32-make run`. 
- Nếu game chạy mượt, map đẹp, animation chuẩn → Chuyển sang Nhiệm vụ 3.
- Nếu game lỗi → Báo họ tự vào nhánh của họ sửa lại, bạn không cần quan tâm.

### Nhiệm vụ 3: Tích hợp và Merge vào `main`
Khi mọi thứ đã ổn, bạn thực hiện "ghép" cành của đồng đội vào thân cây chính:

```bash
# 1. Quay trở về thân cây chính
git checkout main

# 2. Kéo code mới nhất đề phòng có ai vừa cập nhật
git pull origin main

# 3. Kéo nhánh của đồng đội đắp vào nhánh main của bạn
git merge feature/tao-map-rung

# 4. Gắn kết bằng AI (Nếu cần thiết)
# Lúc này bạn nhắn cho AI: "Map đã ở trong repo, cậu hãy update main.c đi"
# AI làm xong, bạn test lại game.

# 5. Đóng gói bản cuối cùng và đẩy lên GitHub (Hoàn tất)
git add .
git commit -m "Đã merge và tích hợp xong map rừng"
git push origin main
```

### 💡 Tóm lại quy trình của Implementer:
`Tải nhánh đồng đội về` ➔ `Test thử` ➔ `Quay về main` ➔ `Gộp nhánh đồng đội vào main (Merge)` ➔ `Nhờ AI xử lý phần code nối` ➔ `Push bản hoàn chỉnh lên GitHub`.
