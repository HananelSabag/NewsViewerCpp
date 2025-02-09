### 📰 **NewsViewerCpp - GUI Version 2.0 ✅**

## 📌 **Project Overview**

NewsViewerCpp is a **C++** application that fetches and displays news articles from **NewsAPI.org**.  
This project was designed to be **efficient, concurrent, and user-friendly**, supporting:
- **Multithreading**
- **Search functionality**
- **Favorites management** ⭐
- **Caching & File Persistence**
- **Graphical User Interface (GUI) with ImGui** 🎨
- **Dark/Light Mode Support** 🌓
- **Full Emoji Support** 😊
- **Customizable Font Sizes** 📏
- **Modern UI Design** 🎯

## 🚀 **Features & Implementations**

✅ **Fetch latest news** dynamically from NewsAPI  
✅ **Search for news** using keywords  
✅ **Graphical User Interface (GUI)** powered by **ImGui**  
✅ **Thread-safe news fetching** with `std::mutex`  
✅ **Efficient caching** to store previous search results  
✅ **Favorites System** – Save, view, and remove favorite articles ⭐  
✅ **File persistence** for favorites & cache  
✅ **Cache expiration mechanism**  
✅ **Error handling & input validation**  
✅ **Dark/Light theme support**  
✅ **Customizable font sizes**  
✅ **Complete Unicode & Emoji Support** 🎉

## 🏠 **Project Structure**

```
NewsViewerCpp/
│── assets/
│   ├── NotoEmoji-Regular.ttf    # Emoji support font
│   └── Roboto-Regular.ttf       # Main font
│── include/
│   ├── news_fetcher.h          # Header file for NewsFetcher class
│   ├── news_storage.h          # Header file for storing favorites
│   └── ui.h                    # Header file for GUI class
│── src/
│   ├── news_fetcher.cpp        # Implements news fetching logic
│   ├── news_storage.cpp        # Implements file storage
│   └── ui.cpp                  # Implements GUI logic
│── main.cpp                    # Entry point
│── CMakeLists.txt             # Build configuration
└── README.md                  # This file
```

## 🛠️ **Dependencies**

- **ImGui** - GUI Framework
- **GLFW** - Window handling
- **OpenGL** - Graphics rendering
- **OpenSSL** - Secure HTTP requests
- **cpp-httplib** - HTTP client
- **nlohmann/json** - JSON parsing

## 🔧 **Building & Running**

### Prerequisites:
- C++17 or later
- CMake 3.14+
- OpenGL
- Git (optional)

### Build Steps:

1. **Clone the repository:**
```bash
git clone https://github.com/YOUR_USERNAME/NewsViewerCpp.git
cd NewsViewerCpp
```

2. **Create build directory:**
```bash
mkdir build && cd build
```

3. **Configure and build:**
```bash
cmake ..
cmake --build .
```

4. **Run the application:**
```bash
./NewsViewer
```

## 📱 **Usage Guide**

### Basic Controls:
- 🔍 Use the search bar to find news
- ⭐ Click star icons to favorite articles
- 🌓 Toggle dark/light mode
- ⚙️ Access settings for font size and other options

### Keyboard Shortcuts:
- `Enter` in search bar to search
- `Esc` to close popups
- `Ctrl+F` to focus search bar

## 📄 **License**

This project is licensed under the MIT License.

## 👥 **Authors**

Developed by **Hananel Sabag & Amit Cohen**