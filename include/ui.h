#ifndef NEWS_UI_H
#define NEWS_UI_H

#ifdef _WIN32
#include <winsock2.h>  // Add this line first
#include <windows.h>
#include <GL/gl.h>
#elif __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include "news_fetcher.h"
#include "news_storage.h"
#include "imgui.h"
#include "glfw3.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// Font Awesome icon definitions for better readability
#define ICON_SEARCH     "\uf002"
#define ICON_HOME       "\uf015"
#define ICON_STAR       "\uf005"
#define ICON_STAR_O     "\uf006"
#define ICON_COG        "\uf013"
#define ICON_SUN        "\uf185"
#define ICON_MOON       "\uf186"
#define ICON_PLUS       "\uf067"
#define ICON_MINUS      "\uf068"
// Add with other icon definitions in ui.h
#define ICON_REFRESH    "\uf021"
#define DEFAULT_FONT_SIZE 18.0f


// Application version
#define APP_VERSION "1.0.0"

class NewsUI {
public:
    // Constructor initializes the UI with a NewsFetcher instance
    explicit NewsUI(NewsFetcher& fetcher);
    ~NewsUI();

    // Main entry point to run the UI
    void run();

private:
    // Core data
    NewsFetcher& fetcher;
    std::vector<NewsFetcher::NewsArticle> headlines;
    std::vector<NewsFetcher::NewsArticle> searchResults;
    std::vector<NewsFetcher::NewsArticle> favorites;
    std::optional<NewsFetcher::NewsArticle> selectedArticle;

    // UI state
    struct UIState {
        std::string searchQuery;
        bool showArticlePopup = false;
        bool showFavoritesPopup = false;
        bool showSettings = false;
        bool showHome = true;
        bool isDarkMode = true;
    } state;

    // Message system for user feedback
    struct Message {
        std::string text;
        ImVec4 color;
        double duration;
        double startTime;
    };
    std::vector<Message> messages;

    // Fonts
    ImFont* defaultFont = nullptr;
    ImFont* iconFont = nullptr;

    // Image handling
    struct ImageTexture {
        GLuint textureId = 0;
        int width = 0;
        int height = 0;
        bool isLoaded = false;
    };
    std::unordered_map<std::string, ImageTexture> textureCache;

    // Theme colors
    struct ThemeColors {
        ImVec4 background{0.9f, 0.9f, 0.9f, 1.0f};
        ImVec4 text{0.0f, 0.0f, 0.0f, 1.0f};
        ImVec4 accent{0.2f, 0.6f, 1.0f, 1.0f};
        ImVec4 success{0.2f, 0.8f, 0.2f, 1.0f};
        ImVec4 warning{1.0f, 0.8f, 0.0f, 1.0f};
        ImVec4 error{0.8f, 0.2f, 0.2f, 1.0f};
    } colors;


    // Core UI rendering functions
    void render();
    void renderToolbar();
    void renderNewsContent();
    void renderFooter();
    void renderMessages();

    // Popup rendering
    void renderArticlePopup();
    void renderFavoritesPopup();
    void renderSettingsPopup();

    // UI utilities
    void showMessage(const std::string& text, const ImVec4& color, float duration = 3.0f);
    void updateMessages();
    void centerText(const char* text, float width, float height);
    void pushIcon();
    void popIcon();
    void renderIconButton(const char* icon, const char* tooltip = nullptr,
                          const ImVec4* color = nullptr);

    // Theme and style
    void applyTheme();
    bool initializeFonts();
    bool hasNewUpdates = false;  // Flag to indicate new content is available



    // Image handling
    ImageTexture createTextureFromImageData(const std::vector<unsigned char>& imageData);
    void deleteTexture(ImageTexture& texture);
    void renderImage(NewsFetcher::NewsArticle& article, float maxWidth, float maxHeight);


    // Content management
    void handleSearch();
    void addToFavorites(const NewsFetcher::NewsArticle& article);
    void removeFavorite(const std::string& title);

    // Window management
    void setupWindow(GLFWwindow* window);
    void cleanup(GLFWwindow* window);
    void setWindowIcon(GLFWwindow* window, const char* iconPath);

};

#endif // NEWS_UI_H