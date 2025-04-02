# 🧰 Git & PlatformIO Cheat-Sheet

---

## 🌳 Git Commands

### **Adding & Committing Changes**

- **Stage changes to commit:**
  ```bash
  git add <file>        # Adds specific file
  git add .             # Adds all changes in the current directory and subdirectories
  ```

- **Commit staged changes:**
  ```bash
  git commit -m "Your commit message"
  ```

### **Viewing History**

- **View commit log:**
  ```bash
  git log
  git log --oneline     # Compact log
  ```

### **Branches**

- **List branches:**
  ```bash
  git branch            # Local branches
  git branch -a         # Local & remote branches
  ```

- **Create & switch branches:**
  ```bash
  git checkout -b <branch-name>
  ```

- **Switch existing branch:**
  ```bash
  git checkout <branch-name>
  ```

- **Delete branch:**
  ```bash
  git branch -d <branch-name>    # Safe delete
  git branch -D <branch-name>    # Force delete
  ```

- **Push branch to remote:**
  ```bash
  git push origin <branch-name>
  ```

### **Synchronizing with Remote**

- **Push commits:**
  ```bash
  git push origin main
  ```

- **Pull remote changes (fetch + merge):**
  ```bash
  git pull origin main
  ```

- **Fetch remote changes (without merging):**
  ```bash
  git fetch origin
  ```

### **Undoing Changes**

- **Discard changes, reset to last commit:**
  ```bash
  git reset --hard HEAD
  ```

- **Reset branch to specific commit:**
  ```bash
  git reset --hard <commit_hash>
  ```

- **Undo last commit, keep changes staged:**
  ```bash
  git reset --soft HEAD~1
  ```

- **Revert commit:**
  ```bash
  git revert <commit_hash>
  ```

---

## 🛠️ PlatformIO (pio) Commands

- **Basic build:**
  ```bash
  pio run
  ```

- **Build specific environment:**
  ```bash
  pio run -e esp32dev
  ```

- **Build and upload:**
  ```bash
  pio run -e esp32dev -t upload
  ```

- **Serial monitor:**
  ```bash
  pio device monitor
  ```

- **Build, upload, and monitor:**
  ```bash
  pio run -e esp32dev -t upload -t monitor
  ```

- **Clean build:**
  ```bash
  pio run -t clean
  ```

- **List devices:**
  ```bash
  pio device list
  ```

- **Upload filesystem:**
  ```bash
  pio run -t uploadfs
  ```

- **Update PlatformIO:**
  ```bash
  pio upgrade
  pio platform update
  ```

---

## 📖 Glossary

- **Branch:** A parallel version of a repository to develop features isolated from main work.
- **Checkout:** Switching branches or restoring working files.
- **Commit:** A snapshot of your project, marking changes in your repository.
- **Fetch:** Downloads latest data from remote without merging it into local branches.
- **Merge:** Combines changes from different branches.
- **Pull:** Fetches remote changes and immediately merges them into your current branch.
- **Push:** Uploads local commits to a remote repository.
- **Remote:** Online repository hosted on platforms like GitHub, GitLab, etc.
- **Reset (hard):** Discards all uncommitted changes, reverting files to specified commit.
- **Reset (soft):** Moves commit history pointer, preserving changes in the staging area.
- **Revert:** Undoes a commit by creating a new commit with inverse changes.
- **Stage:** Prepare files or changes for a commit.

### PlatformIO-Specific

- **Environment (`-e`):** Specific board/platform configurations in `platformio.ini`.
- **Target (`-t`):** Specifies specific action/task for PlatformIO to perform (upload, clean, etc.).
- **Upload:** Transfers built firmware to your device.
- **Monitor:** Views real-time serial output from your device.
- **Filesystem upload (`uploadfs`):** Sends SPIFFS/LittleFS filesystem data to device storage.

