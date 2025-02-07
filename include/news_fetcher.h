#pragma once
#ifndef NEWS_FETCHER_H
#define NEWS_FETCHER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <ctime>
#include "httplib.h"
#include "json.hpp"

class NewsFetcher {
public:
    explicit NewsFetcher(const std::string& apiKey);

    std::vector<std::string> fetchHeadlines();
    std::vector<std::string> searchNews(const std::string& keyword);

private:
    std::string apiKey;
    const std::string baseUrl = "https://newsapi.org/v2/top-headlines";

    std::mutex fetchMutex;
    std::unordered_map<std::string, std::vector<std::string>> searchCache;
    std::unordered_map<std::string, time_t> cacheTimestamps;

    std::string makeRequest(const std::string& query);
    std::string urlEncode(const std::string& value);
    std::string formatNewsArticle(const std::string& title, const std::string& keyword = "");

    void saveCacheToFile();
    void loadCacheFromFile();
    bool isCacheExpired(const std::string& keyword);
};

#endif // NEWS_FETCHER_H
