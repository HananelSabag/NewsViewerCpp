#include "news_fetcher.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <atomic>

using json = nlohmann::json;
namespace fs = std::filesystem;

// Constants for caching configuration
const std::string CACHE_FILE = "news_cache.txt";
const time_t CACHE_EXPIRY = 60 * 60; // 1 Hour cache expiry

// Parse JSON article data into NewsArticle struct
NewsFetcher::NewsArticle NewsFetcher::parseArticleJson(const json& articleJson) {
    // Helper function to safely get value from JSON
    auto safeGet = [](const json& j, const char* key) -> std::string {
        if (j.contains(key) && !j[key].is_null()) {
            return j[key].get<std::string>();
        }
        return "";
    };

    // Get source name safely
    std::string sourceName;
    if (articleJson.contains("source") && !articleJson["source"].is_null()) {
        sourceName = safeGet(articleJson["source"], "name");
    }

    return NewsArticle(
            safeGet(articleJson, "title"),
            safeGet(articleJson, "description"),
            safeGet(articleJson, "content"),
            safeGet(articleJson, "url"),
            sourceName,
            safeGet(articleJson, "publishedAt"),
            safeGet(articleJson, "urlToImage")
    );
}

// Constructor initializes fetcher and loads cache
NewsFetcher::NewsFetcher(const std::string& apiKey) : apiKey(apiKey) {
    loadCacheFromFile();
}

// Fetch top headlines with full article details
std::vector<NewsFetcher::NewsArticle> NewsFetcher::fetchHeadlines() {
    std::lock_guard<std::mutex> lock(fetchMutex);
    std::string query = "/v2/top-headlines?country=us&apiKey=" + apiKey;

    // Return cached results if valid
    if (searchCache.count("top_headlines") && !isCacheExpired("top_headlines")) {
        return searchCache["top_headlines"];
    }

    std::string response = makeRequest(query);
    std::vector<NewsArticle> articles;

    try {
        auto jsonData = json::parse(response);
        if (jsonData.contains("articles")) {
            for (const auto& article : jsonData["articles"]) {
                articles.push_back(parseArticleJson(article));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] JSON Parsing failed: " << e.what() << std::endl;
    }

    // Cache results and save
    searchCache["top_headlines"] = articles;
    cacheTimestamps["top_headlines"] = std::time(nullptr);
    saveCacheToFile();

    return articles;
}

// Search news by keyword and return full article details
std::vector<NewsFetcher::NewsArticle> NewsFetcher::searchNews(const std::string& keyword) {
    if (keyword.length() < 2) {
        std::cerr << "[INFO] Please enter at least 2 characters for search.\n";
        return {};
    }

    std::lock_guard<std::mutex> lock(fetchMutex);

    // Convert to lowercase for case-insensitive search
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

    // Return cached results if valid
    if (searchCache.count(lowerKeyword) && !isCacheExpired(lowerKeyword)) {
        return searchCache[lowerKeyword];
    }

    std::string query = "/v2/everything?q=" + urlEncode(keyword) + "&apiKey=" + apiKey;
    std::string response = makeRequest(query);
    std::vector<NewsArticle> articles;

    try {
        auto jsonData = json::parse(response);
        if (jsonData.contains("articles")) {
            for (const auto& article : jsonData["articles"]) {
                articles.push_back(parseArticleJson(article));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] JSON Parsing failed: " << e.what() << std::endl;
    }

    // Cache results and save
    searchCache[lowerKeyword] = articles;
    cacheTimestamps[lowerKeyword] = std::time(nullptr);
    saveCacheToFile();

    return articles;
}

// Get full article details by title
NewsFetcher::NewsArticle NewsFetcher::getArticleByTitle(const std::string& title) const {
    // Search through all cached articles
    for (const auto& [key, articles] : searchCache) {
        for (const auto& article : articles) {
            if (article.title == title) {
                return article;
            }
        }
    }
    return NewsArticle(); // Return empty article if not found
}

// Save current cache state to file
void NewsFetcher::saveCacheToFile() {
    std::ofstream outFile(CACHE_FILE);
    if (outFile.is_open()) {
        json cacheJson;

        // Convert cache to JSON format
        for (const auto& [key, articles] : searchCache) {
            json articlesArray = json::array();
            for (const auto& article : articles) {
                json articleJson = {
                        {"title", article.title},
                        {"description", article.description},
                        {"content", article.content},
                        {"url", article.url},
                        {"source", article.source},
                        {"publishedAt", article.publishedAt},
                        {"urlToImage", article.urlToImage}
                };
                articlesArray.push_back(articleJson);
            }
            cacheJson[key] = {
                    {"timestamp", cacheTimestamps[key]},
                    {"articles", articlesArray}
            };
        }

        outFile << cacheJson.dump(4);
        outFile.close();
    }
}

// Load cached data from file
void NewsFetcher::loadCacheFromFile() {
    if (!fs::exists(CACHE_FILE)) return;

    std::ifstream inFile(CACHE_FILE);
    if (!inFile.is_open()) return;

    try {
        json cacheJson = json::parse(inFile);

        // Parse JSON cache data
        for (auto& [key, value] : cacheJson.items()) {
            cacheTimestamps[key] = value["timestamp"];
            std::vector<NewsArticle> articles;

            for (const auto& articleJson : value["articles"]) {
                articles.push_back(parseArticleJson(articleJson));
            }

            searchCache[key] = articles;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to load cache: " << e.what() << std::endl;
    }
}

// Check if cached data has expired
bool NewsFetcher::isCacheExpired(const std::string& keyword) {
    return std::difftime(std::time(nullptr), cacheTimestamps[keyword]) > CACHE_EXPIRY;
}

// Make HTTP request to fetch news data
std::string NewsFetcher::makeRequest(const std::string& query) {
    httplib::SSLClient client("newsapi.org");
    auto res = client.Get(query.c_str());

    if (res && res->status == 200) {
        return res->body;
    } else {
        std::cerr << "[ERROR] Failed to fetch data. Status Code: "
                  << (res ? std::to_string(res->status) : "No Response") << std::endl;
        return "{}";
    }
}

/**
* Encodes a string for use in a URL query parameter
* Converts special characters to percent-encoded format
* @param value The string to encode
* @return The URL-encoded string
*/
std::string NewsFetcher::urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (const char c : value) {
        // Keep alphanumeric and certain special chars as-is
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            // Convert other chars to percent-encoded hex
            escaped << '%' << std::setw(2) << std::uppercase << int((unsigned char)c);
        }
    }
    return escaped.str();
}

/**
* Starts the background auto-update thread
* @param intervalSeconds Time between updates in seconds (default 300)
*/
void NewsFetcher::startAutoUpdate(int intervalSeconds) {
    // Prevent multiple update threads
    if (isAutoUpdateRunning) return;

    isAutoUpdateRunning = true;
    updateThread = std::thread(&NewsFetcher::autoUpdateLoop, this, intervalSeconds);
    updateThread.detach();  // Thread runs independently from main thread
}

/**
* Stops the auto-update thread gracefully
*/
void NewsFetcher::stopAutoUpdate() {
    isAutoUpdateRunning = false;
}

/**
* Background auto-update loop that periodically refreshes news content
* @param intervalSeconds The time interval between updates in seconds
*/
void NewsFetcher::autoUpdateLoop(int intervalSeconds) {
    while (isAutoUpdateRunning) {
        // Record start time for timing purposes
        auto startTime = std::chrono::steady_clock::now();

        try {
            std::cout << "[INFO] Starting auto-update..." << std::endl;

            // First update the main headlines
            auto newHeadlines = fetchHeadlines();

            // Then update any cached search results
            for (const auto& [keyword, _] : searchCache) {
                // Skip updating headlines since we just did that
                if (keyword != "top_headlines") {
                    searchNews(keyword);
                }
            }

            std::cout << "[INFO] Auto-update completed successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Auto-update failed: " << e.what() << std::endl;
        }

        // Wait for next update with countdown display
        for (int i = intervalSeconds; i > 0; --i) {
            std::cout << "\r[INFO] Next update in: " << i << " seconds   " << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "\r[INFO] Updating now...                              " << std::endl;
    }
}
