#include "news_fetcher.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <atomic>
NewsFetcher::NewsArticle::NewsArticle(
        const std::string& _title,
        const std::string& _description,
        const std::string& _content,
        const std::string& _url,
        const std::string& _source,
        const std::string& _publishedAt,
        const std::string& _urlToImage
) : title(_title),
    description(_description),
    content(_content),
    url(_url),
    source(_source),
    publishedAt(_publishedAt),
    urlToImage(_urlToImage),
    isImageLoading(false),
    loadError(false),
    isImageLoaded(false),
    hasFullContent(!_content.empty() || !_description.empty())
{
}

using json = nlohmann::json;
namespace fs = std::filesystem;

// Constants for caching configuration
const std::string CACHE_FILE = "news_cache.txt";
const time_t CACHE_EXPIRY = 60 * 60; // 1 Hour cache expiry

/**
 * Constructor initializes fetcher and loads cache
 * @param apiKey The API key for NewsAPI access
 */
NewsFetcher::NewsFetcher(const std::string& apiKey) : apiKey(apiKey) {
    loadCacheFromFile();
}


/**
 * Parse JSON article data into NewsArticle struct with improved source handling
 * @param articleJson JSON object containing article data from NewsAPI
 * @return NewsArticle object with parsed data
 */
NewsFetcher::NewsArticle NewsFetcher::parseArticleJson(const json& articleJson) {
    // Helper function to safely get value from JSON
    auto safeGet = [](const json& j, const char* key) -> std::string {
        if (j.contains(key) && !j[key].is_null()) {
            return j[key].get<std::string>();
        }
        return "";
    };

    // Improved source name extraction
    std::string sourceName;
    if (articleJson.contains("source") && !articleJson["source"].is_null()) {
        const auto& source = articleJson["source"];
        if (source.contains("name") && !source["name"].is_null()) {
            sourceName = source["name"].get<std::string>();
        } else if (source.contains("id") && !source["id"].is_null()) {
            // Fallback to ID if name is not available
            sourceName = source["id"].get<std::string>();
        }
    }

    // Create article with all available data
    auto article = NewsArticle(
            safeGet(articleJson, "title"),
            safeGet(articleJson, "description"),
            safeGet(articleJson, "content"),
            safeGet(articleJson, "url"),
            sourceName,
            safeGet(articleJson, "publishedAt"),
            safeGet(articleJson, "urlToImage")
    );

    // Add additional metadata for UI handling
    article.hasFullContent = !article.content.empty() || !article.description.empty();

    return article;
}

/**
 * Fetches the latest headlines from the NewsAPI.
 * Implements caching to reduce API calls and loads article images asynchronously.
 * @return Vector of NewsArticle objects containing the latest headlines
 */


std::vector<NewsFetcher::NewsArticle> NewsFetcher::fetchHeadlines() {
    std::lock_guard<std::mutex> lock(fetchMutex);
    std::string query = "/v2/top-headlines?country=us&apiKey=" + apiKey;

    if (searchCache.count("top_headlines") && !isCacheExpired("top_headlines")) {
        std::cout << "\n[DEBUG] Returning headlines from cache\n";
        return searchCache["top_headlines"];
    }

    std::string response = makeRequest(query);
    std::cout << "\n[DEBUG] Raw API Response:\n" << response.substr(0, 1000) << "...\n";

    std::vector<NewsArticle> articles;

    try {
        auto jsonData = json::parse(response);
        std::cout << "\n[DEBUG] Parsed " << (jsonData.contains("articles") ?
                                             std::to_string(jsonData["articles"].size()) : "0") << " articles\n";

        if (jsonData.contains("articles")) {
            for (const auto& article : jsonData["articles"]) {
                articles.push_back(parseArticleJson(article));

                // Debug output for each article
                const auto& lastArticle = articles.back();
                std::cout << "\n[DEBUG] Parsed Article:"
                          << "\nTitle: " << lastArticle.title
                          << "\nSource: " << lastArticle.source
                          << "\nPublished: " << lastArticle.publishedAt
                          << "\nDesc Length: " << lastArticle.description.length()
                          << "\nContent Length: " << lastArticle.content.length()
                          << "\nImage URL Length: " << lastArticle.urlToImage.length()
                          << "\n-----------------\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] JSON Parsing failed: " << e.what() << std::endl;
    }

    searchCache["top_headlines"] = articles;
    cacheTimestamps["top_headlines"] = std::time(nullptr);
    saveCacheToFile();

    // Debug cache after save
    std::cout << "\n[DEBUG] Cache after save:\n";
    debugCacheContent("top_headlines");

    return articles;
}

// Add this helper function for debugging cache content
void NewsFetcher::debugCacheContent(const std::string& key) {
    if (!searchCache.count(key)) {
        std::cout << "[DEBUG] No cache found for key: " << key << "\n";
        return;
    }

    const auto& articles = searchCache[key];
    std::cout << "[DEBUG] Cache for " << key << " contains " << articles.size() << " articles\n";

    for (size_t i = 0; i < articles.size(); ++i) {
        const auto& article = articles[i];
        std::cout << "\nArticle " << i + 1 << ":"
                  << "\nTitle: " << article.title
                  << "\nSource: " << article.source
                  << "\nPublished: " << article.publishedAt
                  << "\nDesc Length: " << article.description.length()
                  << "\nContent Length: " << article.content.length()
                  << "\nImage URL: " << (article.urlToImage.empty() ? "None" : article.urlToImage)
                  << "\nImage Loaded: " << (article.isImageLoaded ? "Yes" : "No")
                  << "\nImage Data Size: " << article.imageData.size()
                  << "\n-----------------\n";
    }
}


/**
 * Get article by its title from the cache
 * @param title The title to search for
 * @return Article object, or empty article if not found
 */
bool NewsFetcher::getArticleByTitle(const std::string& title) const {
    for (const auto& [key, articles] : searchCache) {
        for (const auto& article : articles) {
            if (article.title == title) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Searches for news articles based on a keyword
 * @param keyword The search term
 * @return Vector of matching articles
 */
std::vector<NewsFetcher::NewsArticle> NewsFetcher::searchNews(const std::string& keyword) {
    if (keyword.length() < 2) {
        return {};
    }

    std::lock_guard<std::mutex> lock(fetchMutex);
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

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

    searchCache[lowerKeyword] = articles;
    cacheTimestamps[lowerKeyword] = std::time(nullptr);
    saveCacheToFile();


    return articles;
}

/**
 * URL encodes a string for use in HTTP requests
 */
std::string NewsFetcher::urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << std::uppercase << int((unsigned char)c);
        }
    }
    return escaped.str();
}
/**
 * @brief Starts the automatic update process in a background thread
 * Initializes and starts a background thread that periodically fetches new articles.
 * If an auto-update process is already running, this function will return without doing anything.
 */
void NewsFetcher::startAutoUpdate(int intervalSeconds) {
    if (isAutoUpdateRunning) return;
    isAutoUpdateRunning = true;
    updateThread = std::thread(&NewsFetcher::autoUpdateLoop, this, intervalSeconds);
    updateThread.detach();
}


/**
 * @brief Stops the automatic update process safely
 *
 * Sets the atomic flag to stop the background update thread. The thread will complete
 * its current update cycle (if any) before terminating.
 */
void NewsFetcher::stopAutoUpdate() {
    isAutoUpdateRunning = false;
}

std::string NewsFetcher::makeRequest(const std::string& query) {
    httplib::SSLClient client("newsapi.org");
    auto res = client.Get(query.c_str());
    return (res && res->status == 200) ? res->body : "{}";
}

// Cache management implementation
void NewsFetcher::saveCacheToFile() {
    std::ofstream outFile(CACHE_FILE);
    if (outFile.is_open()) {
        json cacheJson;
        for (const auto& [key, articles] : searchCache) {
            json articlesArray = json::array();
            for (const auto& article : articles) {
                articlesArray.push_back({
                                                {"title", article.title},
                                                {"description", article.description},
                                                {"content", article.content},
                                                {"url", article.url},
                                                {"source", article.source},
                                                {"publishedAt", article.publishedAt},
                                                {"urlToImage", article.urlToImage}
                                        });
            }
            cacheJson[key] = {
                    {"timestamp", cacheTimestamps[key]},
                    {"articles", articlesArray}
            };
        }
        outFile << cacheJson.dump(4);
    }
}


//Load Articles from cache
void NewsFetcher::loadCacheFromFile() {
    std::lock_guard<std::mutex> lock(fetchMutex);

    if (!fs::exists(CACHE_FILE)) {
        std::cout << "[INFO] No cache file exists, will create new one\n";
        return;
    }

    try {
        std::ifstream inFile(CACHE_FILE);
        if (!inFile.is_open()) {
            std::cerr << "[ERROR] Failed to open cache file\n";
            return;
        }

        std::string jsonContent((std::istreambuf_iterator<char>(inFile)),
                                std::istreambuf_iterator<char>());

        if (jsonContent.empty()) {
            std::cout << "[INFO] Cache file is empty\n";
            return;
        }

        json cacheJson = json::parse(jsonContent);

        for (auto& [key, value] : cacheJson.items()) {
            try {
                cacheTimestamps[key] = value["timestamp"];
                std::vector<NewsArticle> articles;
                for (const auto& articleJson : value["articles"]) {
                    articles.push_back(parseArticleJson(articleJson));
                }
                searchCache[key] = articles;
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to parse cache entry: " << e.what() << std::endl;
                continue;  // Skip bad entries but continue processing
            }
        }

        std::cout << "[INFO] Successfully loaded cache with "
                  << searchCache.size() << " entries\n";

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to load cache: " << e.what() << std::endl;
        // Create backup of corrupted cache
        if (fs::exists(CACHE_FILE)) {
            std::string backupName = CACHE_FILE + ".bak";
            try {
                fs::copy_file(CACHE_FILE, backupName, fs::copy_options::overwrite_existing);
                std::cout << "[INFO] Created backup of corrupted cache\n";
            } catch (...) {
                // Ignore backup creation errors
            }
        }
    }
}

/**
 * @brief Checks if cached data for a given keyword has expired
 *
 * Compares the cache timestamp with current time and the expiry duration.
 * Cache entries expire after CACHE_EXPIRY seconds (default: 1 hour).
 *
 * @param keyword The cache key to check
 * @return true if cache is expired, false otherwise
 */
bool NewsFetcher::isCacheExpired(const std::string& keyword) {
    return std::difftime(std::time(nullptr), cacheTimestamps[keyword]) > CACHE_EXPIRY;
}



/**
 * @brief Main loop for the auto-update background thread
 *
 * Continuously fetches new headlines and updates cached searches while isAutoUpdateRunning is true.
 * Runs in a separate thread to avoid blocking the main UI.
 *
 * @param intervalSeconds Time to wait between update cycles
 */
void NewsFetcher::autoUpdateLoop(int intervalSeconds) {
    while (isAutoUpdateRunning) {
        try {
            auto newHeadlines = fetchHeadlines();
            for (const auto& [keyword, _] : searchCache) {
                if (keyword != "top_headlines") {
                    searchNews(keyword);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Auto-update failed: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
    }
}

//Load article image inky when user click on the headline to prevent waste of heep request
bool NewsFetcher::loadArticleImageOnDemand(NewsArticle& article) {
    // If already loaded or loading, no need to do anything
    if (article.isImageLoaded || article.isImageLoading) {
        return true;
    }

    // If no image URL or previous error, return false
    if (article.urlToImage.empty() || article.loadError) {
        return false;
    }

    // Start loading in a new thread
    article.isImageLoading = true;
    std::thread([this, &article]() {
        std::lock_guard<std::mutex> lock(imageMutex);
        if (!loadArticleImage(article)) {
            article.loadError = true;
        }
        article.isImageLoading = false;
    }).detach();

    return true;
}

//
bool NewsFetcher::loadArticleImage(NewsArticle& article) {
    if (article.urlToImage.empty() || article.isImageLoaded) {
        return false;
    }

    try {
        std::string url = article.urlToImage;
        if (url.compare(0, 8, "https://") != 0) {
            std::cerr << "[ERROR] Invalid image URL (not HTTPS): " << url << std::endl;
            return false;
        }

        size_t hostEnd = url.find('/', 8);
        if (hostEnd == std::string::npos) {
            std::cerr << "[ERROR] Invalid image URL format: " << url << std::endl;
            return false;
        }

        std::string host = url.substr(8, hostEnd - 8);
        std::string path = url.substr(hostEnd);

        // Create client with shorter timeout
        httplib::SSLClient client(host);
        client.set_connection_timeout(5);  // 5 seconds connection timeout
        client.set_read_timeout(10);       // 10 seconds read timeout

        auto res = client.Get(path.c_str());

        if (!res || res->status != 200 || res->body.empty()) {
            std::cerr << "[ERROR] Failed to load image from: " << url << std::endl;
            return false;
        }

        // Store image data
        article.imageData = std::vector<unsigned char>(res->body.begin(), res->body.end());
        article.isImageLoaded = true;

        std::cout << "[INFO] Successfully loaded image for: " << article.title << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to load image: " << e.what() <<
                  " URL: " << article.urlToImage << std::endl;
    }
    return false;
}
