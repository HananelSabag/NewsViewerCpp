#ifndef NEWS_UI_H
#define NEWS_UI_H

#include <vector>
#include <string>
#include "news_fetcher.h"

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
    NewsFetcher& fetcher;  // Reference to NewsFetcher for data retrieval

    std::vector<std::string> headlines;  // Stores fetched headlines
    std::vector<std::string> searchResults;  // Stores search results
    std::string searchQuery;  // Stores the current user search input

    /**
     * Initializes ImGui and sets up the window.
     */
    void initImGui();

    /**
     * Renders the main ImGui interface.
     */
    void render();

    /**
     * Cleans up ImGui resources before exiting.
     */
    void cleanup();

    /**
     * Handles user input and searches for news.
     */
    void handleSearch();
};

#endif // NEWS_UI_H
