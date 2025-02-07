#ifndef NEWS_UI_H
#define NEWS_UI_H

#include <vector>
#include <string>
#include "news_fetcher.h"
#include "news_storage.h"
#include "imgui.h"

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
    NewsFetcher& fetcher;  // Reference to NewsFetcher for fetching news
    std::vector<std::string> headlines;  // Stores fetched headlines
    std::vector<std::string> searchResults;  // Stores search results
    std::vector<std::string> favorites;  // Stores favorite headlines
    std::string searchQuery;  // Stores the current user search input

    bool showFavoritesPopup = false;  // Controls visibility of the favorites popup
    bool showHome = true;  // ✅ Controls whether to show "Top Headlines" or search results

    /**
     * Renders the main ImGui interface.
     */
    void render();

    /**
     * Displays the favorites popup.
     */
    void renderFavoritesPopup();

    /**
     * Cleans up ImGui resources before exiting.
     */
    void cleanup();

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
};

#endif // NEWS_UI_H
