# PES VCS (Mini Git in C)

## 📌 Project Overview
PES VCS is a simplified Git-like Version Control System implemented in C.  
The goal of this project is to understand how modern version control systems like Git internally manage files using hashing, object storage, and staging mechanisms.

This system stores files as objects identified by their SHA-256 hash and organizes them into a structure similar to Git, including blobs, trees, and an index (staging area).

---

## 🎯 Objectives
- Understand the internal working of Git
- Implement content-addressable storage
- Learn SHA-256 hashing in C
- Work with file systems and directories
- Develop a command-line interface (CLI)
- Practice modular programming

---

## ⚙️ Features

### 🔹 Blob Objects (File Storage)
- Stores raw file contents
- Each file is hashed using SHA-256
- Stored inside `.pes/objects/`
- Same content is stored only once (deduplication)

---

### 🔹 Tree Objects (Directory Structure)
- Represents directories
- Stores file names, modes, and hashes
- Maintains hierarchical structure

---

### 🔹 Index (Staging Area)
- Temporary area to track changes before commit
- Stored in `.pes/index`
- Contains:
  - File mode
  - File hash
  - Timestamp
  - File path

---

### 🔹 SHA-256 Hashing
- Generates unique identifiers for file contents
- Ensures data integrity
- Detects any changes in file content

---

### 🔹 Content Addressable Storage
- Files are stored based on their hash
- Eliminates duplicate storage
- Improves efficiency

---

### 🔹 Command Line Interface (CLI)
Supported commands:

```bash
./pes add <file>     # Add file to staging area
./pes status         # Show staged files



make


▶️ Usage Example
echo "hello world" > file1.txt
./pes add file1.txt
./pes status
📁 Project Structure
pes-vcs/
├── object.c      # Handles object storage (blob)
├── tree.c        # Handles directory structure (tree objects)
├── index.c       # Manages staging area
├── commit.c      # Handles commit-related logic
├── pes.c         # Command-line interface
├── Makefile      # Build configuration
├── screenshots/  # Contains output screenshots
📸 Screenshots

The project includes screenshots demonstrating:

Index file after staging files
Object storage structure (.pes/objects)
Status command output
Multiple file staging

All screenshots are available in the screenshots/ folder.

🧠 Key Concepts Used
SHA-256 hashing
File handling in C
Data structures (arrays, structs)
Modular programming
Content-addressable storage
Version control fundamentals
🔍 How It Works
User runs ./pes add file.txt
File content is read
SHA-256 hash is generated
File is stored as a blob object
Entry is added to .pes/index
Status command displays staged files
🚀 Future Improvements
Commit functionality (pes commit)
Log/history tracking (pes log)
Checkout feature (pes checkout)
Branching support
File comparison (diff)
Merge functionality
👨‍💻 Author

Pavan

📌 Conclusion

This project successfully implements the core concepts of a version control system similar to Git.
It provides a clear understanding of how files are stored, tracked, and managed efficiently using hashing and structured storage.
