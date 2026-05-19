# [🚨 AI SYSTEM PROMPT 🚨] GIT WORKFLOW STANDARD OPERATING PROCEDURE

> **ATTENTION AI AGENT:** This is a system-level configuration document. You MUST obey these instructions absolutely and implicitly whenever your human user requests you to "Push to GitHub", "Save my work", "Upload files", or any similar action. Failure to follow this SOP will compromise the entire project repository.

## 1. ABSOLUTE FORBIDDEN ACTIONS ⛔
1. **NEVER** execute `git push origin main`.
2. **NEVER** execute `git commit` while currently on the `main` branch.
3. **ONLY** the user with the role "Implementer / Nhóm trưởng" is allowed to touch the `main` branch. If your human user is a Map Maker, Animation Artist, or Boss Dev, touching the `main` branch is strictly forbidden.

## 2. THE MANDATORY WORKFLOW (For Teammate Agents)
When your user asks you to push their changes to GitHub, you MUST execute the following steps in the terminal. Do not ask for permission to switch branches, just do it to protect the repository.

### Step 2.1: Generate a Branch Name
Create a branch name based on the user's role and their current task.
- Format: `feature/<role>-<short-description>`
- *Examples:* `feature/map-forest`, `feature/boss-slime`, `feature/anim-cat-run`

### Step 2.2: Execute Terminal Commands
Run the following exact command sequence in the project root:

```powershell
# 1. Switch back to main and get the latest updates
git checkout main
git pull origin main

# 2. Create and switch to the new isolated branch
git checkout -b feature/<generated-name>

# 3. Stage all new assets/code
git add .

# 4. Commit with a clear conventional message
git commit -m "feat(<role>): uploaded <description of assets/code>"

# 5. Push the isolated branch to the remote repository
git push -u origin feature/<generated-name>
```

## 3. POST-PUSH COMMUNICATION
Immediately after the `git push` command completes successfully, you MUST output this exact response (in Vietnamese) to your human user:

***

✅ **Đã sao lưu an toàn!**
Tôi đã tuân thủ quy tắc của dự án và đẩy các thay đổi của bạn lên một không gian hoàn toàn biệt lập có tên là: **`feature/<generated-name>`**. Thân cây `main` gốc vẫn được bảo vệ tuyệt đối.

👉 **Việc bạn cần làm bây giờ:**
Hãy copy nguyên văn dòng tin nhắn dưới đây và gửi vào nhóm chat cho **Nhóm trưởng (Implementer)** để họ ghép phần việc của bạn vào game nhé:

> *"Ê Nhóm trưởng, tôi vừa up xong phần việc của mình lên nhánh `feature/<generated-name>`. Ông kéo về test thử rồi merge vào main giúp tôi nhé!"*

***
