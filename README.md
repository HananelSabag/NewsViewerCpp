# **News Viewer - C++ Advanced Project**

## **Project Overview**
News Viewer is a comprehensive C++ application that fetches and displays real-time news headlines from **NewsAPI**. Built with modern C++ features and best practices, it showcases advanced programming concepts including multithreading, STL usage, and real-time UI updates.

## **Key Features**
✔ **Real-time News Fetching**
- Live headlines from NewsAPI
- Background auto-updates every 5 minutes
- Smart caching system to reduce API calls

✔ **Advanced User Interface**
- Modern ImGui-based interface
- Dark/Light theme support
- Responsive article cards
- Rich article preview with images
- Toast notification system

✔ **Search & Organization**
- Keyword-based article search
- Favorites management system
- Persistent storage of user preferences
- Intelligent cache management

✔ **System Architecture**
- Multi-threaded design
- Thread-safe data handling
- Robust error handling
- Efficient memory management

## **Technical Implementation**

### **1. STL Usage**
```cpp
// Vector management for articles
std::vector<NewsArticle> articles;

// Efficient caching with unordered_map
std::unordered_map<std::string, std::vector<NewsArticle>> searchCache;

// File operations with filesystem
namespace fs = std::filesystem;
if (fs::exists(CACHE_FILE)) {
    // Cache handling
}
```

### **2. Thread Management**
```cpp
// Background update thread
std::thread updateThread;
std::atomic<bool> isAutoUpdateRunning{false};

// Thread-safe operations
std::mutex fetchMutex;
std::lock_guard<std::mutex> lock(fetchMutex);
```

### **3. Cache System**
- Intelligent caching with 1-hour expiry
- Reduces API calls and improves responsiveness
- Thread-safe implementation
```cpp
const time_t CACHE_EXPIRY = 60 * 60; // 1 Hour cache expiry
bool isCacheExpired(const std::string& keyword);
```

### **4. Error Handling**
- Comprehensive error catching and logging
- Graceful degradation on API failures
- User-friendly error messages
```cpp
try {
    headlines = fetcher.fetchHeadlines();
} catch (const std::exception& e) {
    showMessage("Failed to load headlines", colors.error);
}
```

## **Architecture Overview**
```
📦 NewsViewer
 ┣ 📂 src/
 ┃ ┣ 📜 main.cpp              # Application entry
 ┃ ┣ 📜 news_fetcher.cpp      # API & cache management
 ┃ ┣ 📜 news_storage.cpp      # Data persistence
 ┃ ┗ 📜 ui.cpp                # User interface
 ┣ 📂 include/
 ┃ ┣ 📜 news_fetcher.h        # API declarations
 ┃ ┣ 📜 news_storage.h        # Storage declarations
 ┃ ┗ 📜 ui.h                  # UI declarations
 ┣ 📂 assets/                 # Application resources
 ┗ 📜 CMakeLists.txt         # Build configuration
```

## **Build Requirements**
- C++17 compiler
- CMake 3.14+
- OpenGL & GLFW
- OpenSSL

### **Dependencies**
- **ImGui** - UI framework
- **httplib** - HTTP client
- **nlohmann/json** - JSON parsing
- **STL** - Core functionality

### **Build Instructions**
```bash
# Clone the repository
git clone https://github.com/HananelSabag/NewsViewerCpp.git
cd NewsViewerCpp

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the application
./NewsViewer
```

## **Key Components**

### **1. News Fetcher**
- Handles API communication
- Implements caching logic
- Manages background updates
- Ensures thread safety

### **2. News Storage**
- Manages favorites system
- Handles data persistence
- Implements file I/O
- Validates data integrity

### **3. User Interface**
- ImGui-based rendering
- Theme management
- Popup system
- Image loading and caching
- Toast notifications

## **Threading Model**
1. **Main Thread**
   - UI rendering
   - User interaction
   - Event handling

2. **Update Thread**
   - Background fetching
   - Cache management
   - API communication

3. **Image Loading Thread**
   - Asynchronous image loading
   - Memory management
   - Error handling

## **Cache Management**
```cpp
struct CacheEntry {
    std::vector<NewsArticle> articles;
    std::time_t timestamp;
};

std::unordered_map<std::string, CacheEntry> searchCache;
```

- Smart caching with expiry
- Memory-efficient storage
- Thread-safe access

## **Future Improvements**
🔹 Local offline storage  
🔹 Advanced search filters  
🔹 Multiple news sources  
🔹 Custom theme editor  
🔹 Analytics dashboard

## **Authors**
- **Hananel Sabag**
   - Core architecture
   - Threading implementation
   - UI development

- **Amit Cohen**
   - Data management
   - Cache system
   - Error handling

## **Academic Context**
This project was developed as part of the Advanced C++ course at JCE (Jerusalem College of Engineering), supervised by Mr. Harel Welch.

### **Course Requirements Met**
✅ STL Implementation  
✅ Thread Management  
✅ UI Development  
✅ Error Handling  
✅ Modern C++ Features  
✅ Third-party Library Integration

## **License**
This project is licensed under the MIT License - see the LICENSE file for details.