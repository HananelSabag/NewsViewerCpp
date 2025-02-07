### 📰 **NewsViewerCpp - GUI Version ✅**

## 📌 **Project Overview**

NewsViewerCpp is a **C++** application that fetches and displays news articles from **NewsAPI.org**.  
This project was designed to be **efficient, concurrent, and user-friendly**, supporting:
- **Multithreading**,
- **Search functionality**,
- **Caching & File Persistence**,
- **Graphical User Interface (GUI) with ImGui** 🎨

This version **fully transitions from CLI to GUI** while maintaining the project's robustness! 🚀

---

## 🚀 **Features & Implementations**

✅ **Fetch latest news** dynamically from NewsAPI  
✅ **Search for news** using keywords  
✅ **Graphical User Interface (GUI)** powered by **ImGui**  
✅ **Thread-safe news fetching** with `std::mutex`  
✅ **Efficient caching** to store previous search results (`std::unordered_map`)  
✅ **File persistence** with `fstream` and `filesystem`  
✅ **Cache expiration mechanism** to keep results fresh  
✅ **Error handling & input validation**  
✅ **Formatted search results** with **highlighted keywords**

---

## 🏠 **Project Structure**

```
NewsViewerCpp/
│── include/
│   ├── news_fetcher.h   # Header file for NewsFetcher class
│   ├── ui.h             # Header file for GUI class (ImGui)
│── src/
│   ├── news_fetcher.cpp # Implements news fetching logic
│   ├── ui.cpp           # Implements GUI logic (ImGui)
│── main.cpp             # Entry point for GUI application
│── CMakeLists.txt       # Build configuration (CMake)
│── news_cache.txt       # Cached search results (auto-generated)
│── README.md            # This file
│── lib/                 # Precompiled external libraries
```

---

## 🛠️ **Dependencies**

This project utilizes **third-party libraries** for networking, JSON parsing, and GUI rendering:

| Library                                               | Purpose        | Installation       |
| ----------------------------------------------------- | -------------- | ------------------ |
| [cpp-httplib](https://github.com/yhirose/cpp-httplib) | HTTP Client    | Included in source |
| [nlohmann/json](https://github.com/nlohmann/json)     | JSON Parsing   | Included in source |
| [ImGui](https://github.com/ocornut/imgui)            | GUI Framework  | Included in source |
| [GLFW](https://www.glfw.org/)                        | Window Handling | Installed via CMake |

---

## 🔧 **How to Build & Run**

### 🔹 **Prerequisites:**

- **C++17 or later**
- **CMake** (for project compilation)
- **g++ / clang++ / MSVC** (C++ compiler)
- **GLFW & OpenGL** installed

### 🔹 **Steps:**

1. **Clone the repository:**
   ```sh
   git clone https://github.com/YOUR_GITHUB_USERNAME/NewsViewerCpp.git
   cd NewsViewerCpp
   ```

2. **Configure & Compile using CMake:**
   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```

3. **Run the application:**
   ```sh
   ./NewsViewerCpp
   ```

4. **If using Windows:** Open the project in **CLion**, build using **MinGW**, and run `NewsViewer.exe`.

---

## 📌 **Authors**

Developed by **Hananel Sabag & Amit Cohen**  
📧 Contact: [hananel.sabag@example.com](mailto\:hananel.sabag@example.com) | [amit.cohen@example.com](mailto\:amit.cohen@example.com)

---

##  **License**

This project is **open-source** under the **MIT License**.

---


🚀 The GUI version is **fully functional!**   
