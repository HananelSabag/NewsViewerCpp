#include "news_fetcher.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using json = nlohmann::json;
namespace fs = std::filesystem;

const std::string CACHE_FILE = "news_cache.txt";
const time_t CACHE_EXPIRY = 60 * 60; // 1 Hour Expiry

NewsFetcher::NewsFetcher(const std::string& apiKey) : apiKey(apiKey) {
    loadCacheFromFile();
}

// Fetches top headlines and caches them
std::vector<std::string> NewsFetcher::fetchHeadlines() {
    std::lock_guard<std::mutex> lock(fetchMutex);
    std::string query = "/v2/top-headlines?country=us&apiKey=" + apiKey;

    // Check if cached and not expired
    if (searchCache.count("top_headlines") && !isCacheExpired("top_headlines")) {
        return searchCache["top_headlines"];
    }

    std::string response = makeRequest(query);
    std::vector<std::string> headlines;

    try {
        auto jsonData = json::parse(response);
        if (jsonData.contains("articles")) {
            for (const auto& article : jsonData["articles"]) {
                if (article.contains("title")) {
                    headlines.push_back(article["title"]);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] JSON Parsing failed: " << e.what() << std::endl;
    }

    searchCache["top_headlines"] = headlines;
    cacheTimestamps["top_headlines"] = std::time(nullptr);
    saveCacheToFile();

    return headlines;
}

// Searches news by keyword, caching results
std::vector<std::string> NewsFetcher::searchNews(const std::string& keyword) {
    if (keyword.length() < 2) {
        std::cerr << "[INFO] Please enter at least 2 characters for search.\n";
        return {};
    }

    std::lock_guard<std::mutex> lock(fetchMutex);

    // Convert keyword to lowercase for case-insensitive search
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

    // Check cache first
    if (searchCache.count(lowerKeyword) && !isCacheExpired(lowerKeyword)) {
        return searchCache[lowerKeyword];
    }

    std::string query = "/v2/everything?q=" + urlEncode(keyword) + "&apiKey=" + apiKey;
    std::string response = makeRequest(query);

    std::vector<std::string> searchResults;
    try {
        auto jsonData = json::parse(response);
        if (jsonData.contains("articles")) {
            for (const auto& article : jsonData["articles"]) {
                if (article.contains("title")) {
                    std::string title = article["title"];
                    searchResults.push_back(title);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] JSON Parsing failed: " << e.what() << std::endl;
    }

    // Cache and save results
    searchCache[lowerKeyword] = searchResults;
    cacheTimestamps[lowerKeyword] = std::time(nullptr);
    saveCacheToFile();

    return searchResults;
}

// Saves search cache to a file
void NewsFetcher::saveCacheToFile() {
    std::ofstream outFile(CACHE_FILE);
    if (outFile.is_open()) {
        for (const auto& entry : searchCache) {
            outFile << entry.first << "\n" << cacheTimestamps[entry.first] << "\n";
            for (const auto& news : entry.second) {
                outFile << news << "\n";
            }
            outFile << "###END###\n";
        }
        outFile.close();
    }
}

// Loads search cache from a file
void NewsFetcher::loadCacheFromFile() {
    if (!fs::exists(CACHE_FILE)) return;

    std::ifstream inFile(CACHE_FILE);
    std::string line, keyword;
    time_t timestamp;
    std::vector<std::string> results;

    while (std::getline(inFile, line)) {
        if (line == "###END###") {
            searchCache[keyword] = results;
            cacheTimestamps[keyword] = timestamp;
            results.clear();
        } else if (searchCache.count(line) == 0) {
            keyword = line;
            inFile >> timestamp;
            inFile.ignore();
        } else {
            results.push_back(line);
        }
    }
}

// Checks if cached data is expired
bool NewsFetcher::isCacheExpired(const std::string& keyword) {
    return std::difftime(std::time(nullptr), cacheTimestamps[keyword]) > CACHE_EXPIRY;
}

// Makes an HTTP request to fetch news
std::string NewsFetcher::makeRequest(const std::string& query) {
    httplib::SSLClient client("newsapi.org");  // **Changed to SSLClient**

    auto res = client.Get(query.c_str());

    if (res && res->status == 200) {
        return res->body;
    } else {
        std::cerr << "[ERROR] Failed to fetch data. HTTP Status Code: "
                  << (res ? std::to_string(res->status) : "No Response") << std::endl;
        return "{}";
    }
}

// Encodes special characters for URL queries
std::string NewsFetcher::urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (const char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << std::uppercase << int((unsigned char)c);
        }
    }
    return escaped.str();
}
