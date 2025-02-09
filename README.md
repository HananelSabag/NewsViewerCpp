### 🔹 **Updated README for Your Project**
This README reflects all recent changes and improvements.

---

# **News Viewer - C++ Advanced Project**

## **Project Overview**
News Viewer is a C++ application that fetches and displays the latest news headlines from the **NewsAPI**. It features a graphical user interface (GUI) using **ImGui**, supports searching for articles, marking favorites, and includes an **automatic update system** that refreshes news in the background using **multithreading**.

## **Features**
✔ **Real-time news fetching** – Retrieves the latest news articles from NewsAPI.  
✔ **Search functionality** – Find articles by keywords.  
✔ **Favorites system** – Save and manage favorite articles.  
✔ **Dark/Light mode support** – User preference for UI theme.  
✔ **Automatic background updates** – Fetches the latest news every 5 minutes.  
✔ **Cache system** – Stores previously fetched news to reduce API calls.  
✔ **Clickable articles** – View full details of selected articles.

---

## **Setup & Installation**
### **Prerequisites**
- C++17 or later
- **CMake**
- **OpenGL & GLFW**
- **ImGui (Dear ImGui)**
- **httplib** for HTTP requests
- **nlohmann/json** for parsing JSON data
- **Filesystem (C++17)** for file handling

### **Build Instructions**
1. Clone the repository:
   ```
   git clone https://github.com/your-repo/NewsViewerCpp.git
   cd NewsViewerCpp
   ```
2. Create a **build** directory and run CMake:
   ```
   mkdir build && cd build
   cmake ..
   make
   ```
3. Run the application:
   ```
   ./NewsViewer
   ```

---

## **Project Structure**
```
📂 NewsViewerCpp/
│── 📂 src/            # Source code files
│   ├── main.cpp       # Entry point
│   ├── news_fetcher.cpp # Fetching and caching news
│   ├── news_storage.cpp # Managing favorites
│   ├── ui.cpp         # ImGui-based user interface
│── 📂 include/        # Header files
│   ├── news_fetcher.h # Header for NewsFetcher class
│   ├── news_storage.h # Header for NewsStorage class
│   ├── ui.h           # Header for NewsUI class
│── 📂 assets/         # Fonts, icons, and images
│── CMakeLists.txt     # Build configuration
│── README.md          # Project documentation
```

---

## **Technical Details**
### **1️⃣ Use of Standard Template Library (STL)**
✅ **Vectors (`std::vector`)**: Used for managing articles, favorites, and cached searches.  
✅ **Unordered Maps (`std::unordered_map`)**: Used for **caching** search results efficiently.  
✅ **Filesystem (`std::filesystem`)**: Used for reading/writing **cache** and **favorites** to disk.

📍 *Example:*
```cpp
std::unordered_map<std::string, std::vector<NewsArticle>> searchCache;
std::vector<std::string> favorites = NewsStorage::loadFavoritesFromFile();
```

---

### **2️⃣ Multithreading & Synchronization**
✅ **Threads (`std::thread`)**: The application spawns a **background thread** to periodically fetch new news articles every **5 minutes**.  
✅ **Atomic (`std::atomic<bool>`)**: Used to safely control the **auto-update process**, ensuring it stops when exiting.  
✅ **Mutex (`std::mutex`)**: Ensures **thread safety** when accessing the cache to avoid race conditions.

📍 *Example:*
```cpp
std::mutex fetchMutex;  // Protects cache access
std::atomic<bool> isAutoUpdateRunning = false;
```

🔄 **How does auto-update work?**
- **Every 5 minutes**, the background thread fetches new headlines.
- It prevents the UI from freezing by running **asynchronously**.
- Displays a **countdown timer** in the console showing the time until the next update.

📍 *Example Output in Console:*
```
[INFO] Starting auto-update...
[INFO] Auto-update completed successfully.
[INFO] Next update in: 300 seconds...
[INFO] Updating now...
```

---

### **3️⃣ Third-Party Libraries**
✅ **`httplib`** – For making **secure HTTP requests** to NewsAPI.  
✅ **`nlohmann/json`** – For **parsing JSON** responses from NewsAPI.  
✅ **`ImGui`** – Provides a modern **GUI interface** with a responsive design.

📍 *Example: Fetching news from API using `httplib`*
```cpp
httplib::SSLClient client("newsapi.org");
auto res = client.Get("/v2/top-headlines?country=us&apiKey=" + apiKey);
if (res && res->status == 200) {
    auto jsonData = json::parse(res->body);
}
```

---

## **User Interface (UI)**
- Built using **ImGui** for **fast rendering** and **minimal resource usage**.
- **Dark/Light Mode** toggle.
- **Favorites Management** popup.
- **Resizable window with smooth UI elements.**
- **Improved article display** with clickable elements.

📍 *Example UI Preview:*
```
[ News Viewer ]
-------------------------------------
🔍 Search for News:  [ Search Button ]
-------------------------------------
📰 [Article Title 1] ⭐
📰 [Article Title 2] ⭐
📰 [Article Title 3] ⭐
-------------------------------------
⭐ Favorites | ⚙ Settings | 🌞 Dark Mode
```

---

## **How the Auto-Update System Works**
1️⃣ **User starts the app →** Fetches headlines immediately.  
2️⃣ **Background thread starts →** Fetches new headlines every 5 minutes.  
3️⃣ **If user performs a search →** Results are cached for faster access.  
4️⃣ **If the user exits →** The background thread stops safely.

📍 *Example implementation:*
```cpp
void NewsFetcher::autoUpdateLoop(int intervalSeconds) {
    while (isAutoUpdateRunning) {
        fetchHeadlines(); // Fetches latest news
        std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
    }
}
```

---

## **How Favorites Work**
✔ Articles can be **added** or **removed** from favorites.  
✔ **Favorites are stored** in `favorites.txt`.  
✔ They **persist** even after restarting the application.

📍 *Example implementation:*
```cpp
void NewsStorage::saveFavoritesToFile(const std::vector<std::string>& favorites) {
    std::ofstream outFile("favorites.txt");
    for (const auto& fav : favorites) {
        outFile << fav << std::endl;
    }
}
```

---

## **How Caching Works**
✔ Cached articles **expire after 1 hour**.  
✔ This **reduces API calls** and **improves performance**.  
✔ Stored in **news_cache.txt**.

📍 *Example implementation:*
```cpp
std::unordered_map<std::string, std::vector<NewsArticle>> searchCache;
if (searchCache.count("top_headlines") && !isCacheExpired("top_headlines")) {
    return searchCache["top_headlines"];
}
```

---

## **Authors**
📌 **Hananel Sabag** – Developer  
📌 **Amit Cohen** – Developer  


---

## **Future Improvements**
🔹 Improve UI with **custom styles and animations**.  
🔹 Add **local storage** for reading news offline.  
🔹 Implement **real-time notifications** for breaking news.

---

## **License**
This project is licensed under the **MIT License**.

---

### ✅ **Final Checklist**
✔ **All project requirements met**  
✔ **STL (vector, unordered_map, filesystem) used correctly**  
✔ **Multithreading implemented for auto-updates**  
✔ **Mutex and atomic variables used where necessary**  
✔ **ImGui-based UI fully functional**  
✔ **Favorites and caching system working correctly**
