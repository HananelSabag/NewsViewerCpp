#ifndef NEWS_UI_H
#define NEWS_UI_H

#include <vector>
#include <string>
#include <optional>
#include "news_fetcher.h"
#include "news_storage.h"
#include "imgui.h"

// Font Awesome icon definitions
#define FA_ICON_SEARCH     "\uf002"
#define FA_ICON_HOME       "\uf015"
#define FA_ICON_STAR       "\uf005"
#define FA_ICON_STAR_O     "\uf006"
#define FA_ICON_COG        "\uf013"
#define FA_ICON_SUN        "\uf185"
#define FA_ICON_MOON       "\uf186"
#define FA_ICON_TIMES      "\uf00d"

/**
 * @class NewsUI
 * @brief Handles the graphical user interface for the news viewer application.
 */
class NewsUI {
public:
    /**
     * Constructor - Initializes the UI with a reference to a NewsFetcher instance.
     * @param fetcher Reference to a NewsFetcher object.
     */
    explicit NewsUI(NewsFetcher& fetcher);

    /**
     * Runs the main UI loop using ImGui.
     */
    void run();

private:
    NewsFetcher& fetcher;                                  // Reference to NewsFetcher for fetching news
    std::vector<NewsFetcher::NewsArticle> headlines;       // Stores fetched headlines
    std::vector<NewsFetcher::NewsArticle> searchResults;   // Stores search results
    std::vector<std::string> favorites;                    // Stores favorite headlines (just titles)
    std::string searchQuery;                               // Stores the current user search input

    // Current selected article for popup
    std::optional<NewsFetcher::NewsArticle> selectedArticle;
    bool showArticlePopup = false;                         // Controls visibility of the article details popup

    // UI State variables
    bool showFavoritesPopup = false;   // Controls visibility of the favorites popup
    bool showHome = true;              // Controls whether to show "Top Headlines" or search results
    bool isDarkMode = true;            // Controls dark/light mode
    float fontSize = 16.0f;            // Controls font size
    float pendingFontSize = 14.0f;     // For handling font size changes
    bool fontSizeChanged = false;      // Flag for font changes
    bool showSettings = false;         // Controls settings popup visibility
    ImFont* defaultFont = nullptr;     // Default font
    ImFont* iconFont = nullptr;        // Font Awesome icons font

    // UI Colors
    ImVec4 backgroundColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
    ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 accentColor = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
    ImVec4 buttonColor = ImVec4(0.2f, 0.6f, 1.0f, 0.6f);
    ImVec4 buttonHoverColor = ImVec4(0.2f, 0.6f, 1.0f, 0.8f);

    /**
     * Renders the main ImGui interface.
     */
    void render();

    /**
     * Displays the favorites popup.
     */
    void renderFavoritesPopup();

    /**
     * Displays the settings popup.
     */
    void renderSettingsPopup();

    /**
     * Displays the article details popup.
     */
    void renderArticlePopup();

    /**
     * Renders the main toolbar (search, favorites, settings buttons).
     */
    void renderToolbar();

    /**
     * Renders the news content area.
     */
    void renderNewsContent();

    /**
     * Applies the current theme (dark/light mode).
     */
    void applyTheme();

    /**
     * Loads and initializes fonts.
     */
    bool initializeFonts();

    /**
     * Checks and handles font size changes safely between frames.
     */
    void checkFontSizeChange();

    /**
     * Handles user input and searches for news.
     */
    void handleSearch();

    /**
     * Adds a news headline to favorites and saves it to file.
     * @param headline The news headline to add.
     */
    void addToFavorites(const std::string& headline);

    /**
     * Removes a news headline from favorites and updates the file.
     * @param headline The news headline to remove.
     */
    void removeFavorite(const std::string& headline);

    /**
     * Helper function to push icon font for rendering Font Awesome icons.
     */
    void pushIcon() { ImGui::PushFont(iconFont); }

    /**
     * Helper function to pop icon font after rendering Font Awesome icons.
     */
    void popIcon() { ImGui::PopFont(); }

    /**
     * Renders a button with a Font Awesome icon.
     * @param icon The Font Awesome icon code.
     * @param tooltip Optional tooltip text to show on hover.
     */
    void renderIconButton(const char* icon, const char* tooltip = nullptr);
};

#endif // NEWS_UI_H