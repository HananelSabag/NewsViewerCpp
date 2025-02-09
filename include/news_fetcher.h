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
#include "httplib.h"
#include "json.hpp"
#include <thread>
#include <atomic>

class NewsFetcher {
public:
    // Struct to hold complete article data
    struct NewsArticle {
        std::string title;
        std::string description;
        std::string content;
        std::string url;
        std::string source;
        std::string publishedAt;
        std::string urlToImage;

        // Default constructor
        NewsArticle(const std::string& _title = "",
                    const std::string& _description = "",
                    const std::string& _content = "",
                    const std::string& _url = "",
                    const std::string& _source = "",
                    const std::string& _publishedAt = "",
                    const std::string& _urlToImage = "")
                : title(_title), description(_description), content(_content),
                  url(_url), source(_source), publishedAt(_publishedAt),
                  urlToImage(_urlToImage) {}
    };
    void startAutoUpdate(int intervalSeconds = 300);  // 5 minutes default
    void stopAutoUpdate();

    // Constructor takes API key
    explicit NewsFetcher(const std::string& apiKey);

    // Main public interface
    std::vector<NewsArticle> fetchHeadlines();
    std::vector<NewsArticle> searchNews(const std::string& keyword);
    NewsArticle getArticleByTitle(const std::string& title) const;

private:
    std::string apiKey;
    std::string baseUrl = "https://newsapi.org/v2/top-headlines";
    bool isAutoUpdateRunning = false;
    std::thread updateThread;


    // Cache storage
    std::unordered_map<std::string, std::vector<NewsArticle>> searchCache;
    std::unordered_map<std::string, time_t> cacheTimestamps;
    std::mutex fetchMutex;

    // Helper methods
    void saveCacheToFile();
    void loadCacheFromFile();
    bool isCacheExpired(const std::string& keyword);
    std::string makeRequest(const std::string& query);
    std::string urlEncode(const std::string& value);
    NewsArticle parseArticleJson(const nlohmann::json& articleJson);
    void autoUpdateLoop(int intervalSeconds);
};

#endif // NEWS_FETCHER_H