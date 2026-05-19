# [🚨 AI SYSTEM PROMPT 🚨] AUTO-IMPLEMENTATION SOP (THE "NO-BRAINER" WORKFLOW)

> **ATTENTION AI AGENT:** Your human user has been instructed to work "mất não" (brainless/no-brainer mode). They will simply dump random asset files (PNGs, TMJs) or code snippets into a folder and tell you: *"Implement this into the codebase and push to GitHub."*
> 
> **It is YOUR responsibility as the AI to figure out where those files go, modify the C source code to integrate them, and push safely to a branch.** 

You must strictly follow this SOP when asked to implement assets into the codebase:

## STEP 1: IDENTIFY AND ORGANIZE THE ASSETS
Do not leave files in a random "DropBox" or Desktop folder. You must move them into the project's standard directories:
- **If it's a Map file (`.tmj`) or Tileset (`.png` for maps):** Move them to `assets/` or `LAMO/Final/`.
- **If it's Character/Boss Sprites (`.png` with animation frames):** Create a new folder `assets/sprites/` (if it doesn't exist) and move them there. DO NOT keep using the old `FREE_Cat...` folder.
- **If it's Boss Logic (`.c` / `.h`):** Move them to `src/`.

## STEP 2: IMPLEMENT INTO SOURCE CODE (VIBECODING)
You must modify the codebase to actually USE the new assets.
### 🗺️ For Map Makers (Map/TMJ integration):
1. Open `src/main.c`.
2. Locate the `maps[]` array.
3. Add the new `assets/your_new_map.tmj` path to the array.
4. Ensure the `mapCount` or loop logic accounts for the new map.

### 🎨 For Animation Artists (Sprite integration):
1. Open `src/main.c`.
2. Locate the `LoadTexture()` calls for `texIdle`, `texWalk`, `texRun`, `texJump`, `texAttack`.
3. Replace the old hardcoded paths (`FREE_Cat 2D...`) with the paths to your new sprites in `assets/sprites/`.
4. Open `src/game.c`. Look at the `DrawPlayer()` function.
5. **CRITICAL:** Update the `maxFrames` variables inside the switch-case to match the exact number of frames in the new sprite sheets! If you don't do this, the animation will glitch.

### 👿 For Boss Developers (Code integration):
1. Your user might just give you logic rules or a partial `boss.c`. You must complete it.
2. Open `src/main.c`.
3. `#include "boss.h"` at the top.
4. Add `InitBoss()` before the game loop.
5. Add `UpdateBoss()` inside the game loop.
6. Add `DrawBoss()` inside the `BeginDrawing()` section.
7. *Note: The Makefile already uses `$(wildcard src/*.c)`, so you do not need to edit the Makefile for new `.c` files.*

## STEP 3: VERIFY COMPILATION
Before committing, you MUST compile the game to ensure your code implementation didn't break anything.
Run:
```bash
mingw32-make
```
If it fails, fix your code. Do not push broken C code.

## STEP 4: BRANCH AND PUSH (READ SOP 09)
Once the code compiles and the assets are integrated:
1. You MUST follow `docs/09_AI_AGENT_GIT_SOP.md`.
2. Do not push to `main`. Create a `feature/...` branch.
3. Commit everything (`git add .`, `git commit`).
4. Push the branch.
5. Output the success message to the user asking them to ping the Team Leader.
