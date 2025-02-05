#pragma once
#ifndef NEWS_FETCHER_H
#define NEWS_FETCHER_H

#include <string>
#include <vector>
#include "httplib.h"
#include "json.hpp"

class NewsFetcher {
public:
    // Constructor - Receives an API key to fetch news
    explicit NewsFetcher(const std::string& apiKey);

    // Fetches the latest news headlines from NewsAPI
    std::vector<std::string> fetchHeadlines();

private:
    std::string apiKey;  // API Key for authentication
    const std::string baseUrl = "https://newsapi.org/v2/top-headlines";  // API endpoint

    // Internal function to make the API request
    std::string makeRequest();
};

#endif // NEWS_FETCHER_H
