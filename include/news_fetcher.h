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

class NewsFetcher {
public:
    /**
     * Constructor - Initializes the NewsFetcher with an API key.
     * @param apiKey The API key for the NewsAPI service.
     */
    explicit NewsFetcher(const std::string& apiKey);

    /**
     * Fetches the latest top headlines and caches them.
     * @return A vector of news headlines.
     */
    std::vector<std::string> fetchHeadlines();

    /**
     * Searches for news articles based on a given keyword.
     * @param keyword The search term.
     * @return A vector of relevant news headlines.
     */
    std::vector<std::string> searchNews(const std::string& keyword);

private:
    std::string apiKey;  // API key for authentication
    std::string baseUrl = "https://newsapi.org/v2/top-headlines";  // Base API endpoint

    std::unordered_map<std::string, std::vector<std::string>> searchCache; // Cached search results
    std::unordered_map<std::string, time_t> cacheTimestamps;  // Cache timestamps
    std::mutex fetchMutex;  // Mutex for thread safety

    /**
     * Saves the cached search results to a file.
     */
    void saveCacheToFile();

    /**
     * Loads cached search results from a file.
     */
    void loadCacheFromFile();

    /**
     * Checks if a cached result is expired.
     * @param keyword The keyword or category to check.
     * @return True if expired, otherwise false.
     */
    bool isCacheExpired(const std::string& keyword);

    /**
     * Performs an HTTP request to fetch news data.
     * @param query The API query string.
     * @return The API response in JSON format.
     */
    std::string makeRequest(const std::string& query);

    /**
     * Encodes special characters in a URL query.
     * @param value The string to encode.
     * @return The encoded URL string.
     */
    std::string urlEncode(const std::string& value);
};

#endif // NEWS_FETCHER_H
