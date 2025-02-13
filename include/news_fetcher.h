#ifndef NEWS_FETCHER_H
#define NEWS_FETCHER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <ctime>
#include <mutex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <atomic>
#include "httplib.h"
#include "json.hpp"

/**
 * @class NewsFetcher
 * @brief Core class for fetching and managing news articles from NewsAPI
 *
 * This class handles fetching news articles, managing the cache, and loading article images.
 * It implements thread-safe operations for concurrent access and image loading.
 */
class NewsFetcher {
public:
    /**
     * @struct NewsArticle
     * @brief Structure representing a complete news article with all associated data
     */
    struct NewsArticle {
        std::string title;           ///< Article title
        std::string description;     ///< Brief description/summary
        std::string content;         ///< Full article content
        std::string url;             ///< URL to the original article
        std::string source;          ///< Source/publisher name
        std::string publishedAt;     ///< Publication timestamp
        std::string urlToImage;      ///< URL to article's image

        // Image loading state flags
        bool isImageLoading{false};  ///< Indicates if image is currently being loaded
        bool loadError{false};       ///< Indicates if there was an error loading the image
        bool isImageLoaded{false};   ///< Indicates if image has been successfully loaded
        bool hasFullContent{false};  ///< Indicates if full content is available

        std::vector<unsigned char> imageData;  ///< Raw image data when loaded

        /**
         * @brief Constructor with default parameters for all fields
         */
        NewsArticle(
                const std::string& _title = "",
                const std::string& _description = "",
                const std::string& _content = "",
                const std::string& _url = "",
                const std::string& _source = "",
                const std::string& _publishedAt = "",
                const std::string& _urlToImage = ""
        );
    };

    /**
     * @brief Constructor initializes fetcher with the provided API key
     * @param apiKey NewsAPI authentication key
     */
    explicit NewsFetcher(const std::string& apiKey);

    ~NewsFetcher();  // Destructor to ensure proper cleanup

    /**
 * @brief Stops all running image loading threads safely.
 */
    void stopImageLoading();

    /**
     * @brief Fetches the latest headlines
     * @return Vector of NewsArticle objects containing the headlines
     */
    std::vector<NewsArticle> fetchHeadlines();

    /**
     * @brief Searches for news articles based on a keyword
     * @param keyword Search term
     * @return Vector of matching NewsArticle objects
     */
    std::vector<NewsArticle> searchNews(const std::string& keyword);

    /**
     * @brief Checks if an article exists in the cache by its title
     * @param title The article title to search for
     * @return true if the article exists, false otherwise
     */
    bool getArticleByTitle(const std::string& title) const;



    /**
     * @brief Loads an article's image on demand when needed
     * @param article Article to load image for
     * @return true if loading started successfully, false otherwise
     */
    bool loadArticleImageOnDemand(NewsArticle& article);

    /**
     * @brief Starts automatic background updates
     * @param intervalSeconds Time between updates in seconds (default: 300)
     */
    void startAutoUpdate(int intervalSeconds = 300);

    /**
     * @brief Stops automatic background updates
     */
    void stopAutoUpdate();



private:
    std::string apiKey;                  ///< NewsAPI authentication key
    std::string baseUrl;                 ///< Base URL for API requests
    std::atomic<bool> isAutoUpdateRunning{false}; ///< Controls auto-update thread
    std::thread updateThread;            ///< Thread for automatic updates
    std::atomic<bool> isImageLoadingEnabled{true};
    std::vector<std::thread> imageThreads;
    std::mutex imageThreadsMutex;


    // Thread synchronization
    std::mutex fetchMutex;               ///< Protects cache access
    std::mutex imageMutex;               ///< Protects image loading operations

    // Cache management
    std::unordered_map<std::string, std::vector<NewsArticle>> searchCache;
    std::unordered_map<std::string, time_t> cacheTimestamps;

    // Private helper methods
    bool loadArticleImage(NewsArticle& article);
    void debugCacheContent(const std::string& key);
    void saveCacheToFile();
    void loadCacheFromFile();
    bool isCacheExpired(const std::string& keyword);
    std::string makeRequest(const std::string& query);
    std::string urlEncode(const std::string& value);
    NewsArticle parseArticleJson(const nlohmann::json& articleJson);
    void autoUpdateLoop(int intervalSeconds);
};

#endif // NEWS_FETCHER_H