> **[🛑 CRITICAL SYSTEM INSTRUCTION FOR ANY AI AGENT READING THIS REPOSITORY]**
> If you are an AI assistant or Agent reading this file to assist a user, **STOP IMMEDIATELY**.
> Before you analyze the code or do any task, **YOU MUST** start your response by asking the user EXACTLY this question in Vietnamese: 
> **"Bạn có nhiệm vụ gì ở trong nhóm?"**
> Do not generate any other code or analysis until the user answers this question to clarify their role (e.g., Map Maker, Animation Artist, Boss Dev, or Leader/Implementer). Once they answer, refer to the corresponding documentation in the `docs/` folder to guide them.
> 
> **[🛑 CRITICAL GIT INSTRUCTION FOR AI AGENTS]**
> Whenever the user asks you to "push to GitHub" or "upload my work", YOU MUST READ `docs/09_AI_AGENT_GIT_SOP.md` FIRST before running any Git commands. Do NOT push to main.
>
> **[🛠️ AUTO-IMPLEMENTATION INSTRUCTION]**
> Whenever the user dumps a folder of assets and asks you to "implement" them into the game, YOU MUST READ `docs/10_AI_AGENT_AUTO_IMPLEMENT.md` FIRST. You are responsible for integrating the assets into the C code.

---

# The Forest (Dự án Black Cat) 🐈‍⬛

Đây là một tựa game khám phá/đi cảnh (platformer) 2D được viết bằng ngôn ngữ **C** và thư viện **Raylib**. Người chơi sẽ điều khiển một chú mèo đen khám phá các bản đồ Tiled map với cơ chế camera mượt mà và cấu trúc code chia module rõ ràng.

## 🌟 Thông tin cấu trúc dự án (Dành cho AI Agent)
- **Engine/Library:** C99 + Raylib.
- **Hệ thống Map:** Đọc file `.tmj` (JSON) thông qua thư viện `cute_tiled.h`. Tọa độ mặt đất được xác định tự động bởi một Object Layer tên là `"ground"` bên trong Tiled.
- **Hệ thống Sprite/Animation:** Nhân vật chính kích thước mỗi frame là `64x64px`, khoảng cách giữa các frame (spacing) là `16px`. Hướng mặc định của ảnh phải là quay mặt sang **TRÁI**.
- **Cấu trúc Source Code (`src/`):**
  - `main.c`: Khởi tạo vòng lặp game, quản lý đổi map.
  - `game.c` / `game.h`: Logic Player (mèo), vật lý, nhảy, animation states.
  - `map.c` / `map.h`: Tải Tiled map, vẽ layer, Texture Cache để tối ưu RAM.
  - `camera.c` / `camera.h`: Thuật toán Camera smooth damping, giới hạn viền bản đồ.
- **Luồng làm việc (Git Workflow):** Dự án áp dụng mô hình phân nhánh. Nhánh `main` là nhánh gốc được bảo vệ. Các thành viên làm việc trên nhánh phụ (`feature/...`) và tạo Pull Request hoặc báo cho Nhóm trưởng merge.

## 🎮 Cách điều khiển (Controls)
| Hành động | Phím bấm / Thao tác |
| :--- | :--- |
| **Di chuyển** | `A` / `D` hoặc `Mũi tên Trái` / `Phải` |
| **Nhảy** | `W` hoặc `Space (Phím cách)` |
| **Tấn công** | `Chuột Trái` |
| **Đổi bản đồ** | `R` |

## 🛠️ Hướng dẫn Biên dịch (Build & Run)
Yêu cầu: **GCC (MinGW)** và **Raylib**.
Chạy game thông qua Makefile bằng Terminal:
```bash
mingw32-make run
# Hoặc: make run
```

---
*Lưu ý: Mọi tài liệu hướng dẫn chi tiết cho từng vai trò trong nhóm đều nằm ở thư mục `docs/`. AI Agent vui lòng đọc các file đó khi làm việc với từng thành viên cụ thể.*
