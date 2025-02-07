# 📰 NewsViewerCpp - CLI Version ✅

## 📌 Project Overview

NewsViewerCpp is a **C++** application that fetches and displays news articles from **NewsAPI.org**.\
The project is designed to be **efficient, concurrent, and user-friendly**, supporting **search functionality, caching, and multithreading**.

---

## 🚀 Features & Implementations

- **Fetch latest news** from NewsAPI
- **Search for news** by keyword
- **Thread-safe requests** using `std::mutex`
- **Efficient caching** to store previous search results (`std::unordered_map`)
- **File persistence** with `fstream` and `filesystem`
- **Cache expiration mechanism** to keep results fresh
- **Error handling & input validation**
- **Formatted search results** with **highlighted keywords**

---

## 🏠 Project Structure

```
NewsViewerCpp/
│── include/
│   ├── news_fetcher.h   # Header file for NewsFetcher class
│── src/
│   ├── news_fetcher.cpp # Implements news fetching logic
│── main.cpp             # Entry point for CLI application
│── CMakeLists.txt       # Build configuration
│── news_cache.txt       # Cached search results (auto-generated)
│── README.md            # This file
```

---

## 🛠️ Dependencies

This project utilizes **third-party libraries** for networking and JSON parsing:

| Library                                               | Purpose      | Installation       |
| ----------------------------------------------------- | ------------ | ------------------ |
| [cpp-httplib](https://github.com/yhirose/cpp-httplib) | HTTP Client  | Included in source |
| [nlohmann/json](https://github.com/nlohmann/json)     | JSON Parsing | Included in source |

---

## 🔧 How to Build & Run

### 🔹 Prerequisites:

- **C++17 or later**
- **CMake** (for project compilation)
- **g++ / clang++** or **MSVC** (C++ compiler)

### 🔹 Steps:

1. **Clone the repository:**
   ```sh
   git clone https://github.com/YOUR_GITHUB_USERNAME/NewsViewerCpp.git
   cd NewsViewerCpp
   ```
2. **Create build directory and compile:**
   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```
3. **Run the application:**
   ```sh
   ./NewsViewerCpp
   ```

---

## 📌 Authors

Developed by **Hananel Sabag & Amit Cohen**\
📧 Contact: [hananel.sabag@example.com](mailto\:hananel.sabag@example.com) | [amit.cohen@example.com](mailto\:amit.cohen@example.com)

---

## 💜 License

This project is **open-source** under the **MIT License**.

---

### 🎨 Next Steps - GUI Integration 🎨

🚀 The CLI version is **complete!** 🎉\
Next, we'll integrate **ImGui** for a **graphical interface**! 🖥️\
Stay tuned! 🔥

