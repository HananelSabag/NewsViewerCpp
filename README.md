# 📰 NewsViewerCpp - GUI Version ✅

## 📌 Project Overview

**NewsViewerCpp** is a **C++** application that fetches and displays news articles from **NewsAPI.org**.  
This project was designed to be **efficient, concurrent, and user-friendly**, supporting:
- **Multithreading**
- **Search functionality**
- **Favorites management** ⭐
- **Caching & File Persistence**
- **Graphical User Interface (GUI) with ImGui** 🎨

This version **fully transitions from CLI to GUI** while maintaining the project's robustness! 🚀

---

## 🚀 Features & Implementations

✅ **Fetch latest news** dynamically from NewsAPI  
✅ **Search for news** using keywords  
✅ **Graphical User Interface (GUI)** powered by **ImGui**  
✅ **Thread-safe news fetching** with `std::mutex`  
✅ **Efficient caching** to store previous search results (`std::unordered_map`)  
✅ **Favorites System** – Save, view, and remove favorite articles ⭐  
✅ **File persistence** for favorites & cache with `fstream` and `filesystem`  
✅ **Cache expiration mechanism** to keep results fresh  
✅ **Error handling & input validation**  
✅ **Formatted search results** with **highlighted keywords**  
✅ **Unicode & Emoji Support** 🎉  

---

## 🏠 Project Structure

NewsViewerCpp/
│── assets/
│   ├── NotoEmoji-Regular.ttf  # Emoji support font
│── include/
│   ├── news_fetcher.h         # Header file for NewsFetcher class
│   ├── news_storage.h         # Header file for storing favorites
│   ├── ui.h                   # Header file for GUI class (ImGui)
│── src/
│   ├── news_fetcher.cpp       # Implements news fetching logic
│   ├── news_storage.cpp       # Implements file storage for favorites
│   ├── ui.cpp                 # Implements GUI logic (ImGui)
│── main.cpp                   # Entry point for GUI application
│── CMakeLists.txt             # Build configuration (CMake)
│── news_cache.txt             # Cached search results (auto-generated)
│── favorites.txt              # Stored favorite news articles (auto-generated)
│── README.md                  # This file
│── lib/                       # Precompiled external libraries

---

## 🛠️ Dependencies

This project utilizes **third-party libraries** for networking, JSON parsing, and GUI rendering:

| Library                                               | Purpose               | Installation        |
| ----------------------------------------------------- | --------------------- | ------------------ |
| [cpp-httplib](https://github.com/yhirose/cpp-httplib) | HTTP Client          | Included in source |
| [nlohmann/json](https://github.com/nlohmann/json)     | JSON Parsing         | Included in source |
| [ImGui](https://github.com/ocornut/imgui)            | GUI Framework        | Included in source |
| [GLFW](https://www.glfw.org/)                        | Window Handling      | Installed via CMake |
| [OpenSSL](https://www.openssl.org/)                  | Secure HTTP Requests | Installed via CMake |

---

## 🔧 How to Build & Run

### 🔹 Prerequisites:

- **C++17 or later**
- **CMake** (for project compilation)
- **g++ / clang++ / MSVC** (C++ compiler)
- **GLFW & OpenGL** installed
- **Noto Emoji Font** (Included in `assets/`)

### 🔹 Steps:

1. **Clone the repository:**
   ```sh
   git clone https://github.com/YOUR_GITHUB_USERNAME/NewsViewerCpp.git
   cd NewsViewerCpp

	2.	Configure & Compile using CMake:

mkdir build && cd build
cmake ..
make


	3.	Run the application:

./NewsViewerCpp


	4.	If using Windows: Open the project in CLion, build using MinGW, and run NewsViewer.exe.

📌 Favorites System - How it Works? ⭐

Adding to Favorites
	•	Click the ⭐ button next to any news article.

Viewing Favorites
	•	Click the “Favorites” button to open the Favorites Popup.

Removing from Favorites
	•	Click the ❌ button next to any saved article to remove it.

Persistent Storage
	•	Favorite articles are saved to favorites.txt and will be reloaded on the next launch.

📌 Cache System - How it Works?
	•	News API requests are cached to reduce redundant API calls.
	•	Cached data expires after 1 hour and is automatically refreshed.
	•	Cached data is stored in news_cache.txt to ensure efficiency.

📌 Graphical User Interface (GUI) Overview

✅ Search Bar – Search for specific news articles
✅ Headlines Section – Displays the top news articles
✅ Search Results – Displays results when searching for news
✅ Favorites Popup – View & manage saved articles
✅ Dark & Light Mode – Improved UI aesthetics
✅ Unicode & Emoji Support – Display icons for better visualization

📌 Authors

Developed by Hananel Sabag & Amit Cohen
📧 Contact: hananel12345@gmail.com | cohen.amit24@gmail.com

License

This project is open-source under the MIT License.

🎨 Final Notes

🚀 The GUI version is fully functional! 🎉
If you encounter any issues, please ensure you have all dependencies installed correctly!

