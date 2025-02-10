#ifndef NEWS_STORAGE_H
#define NEWS_STORAGE_H

#include <vector>
#include <string>
#include "news_fetcher.h"

class NewsStorage {
public:
    static void saveFavoritesToFile(const std::vector<NewsFetcher::NewsArticle>& favorites);
    static std::vector<NewsFetcher::NewsArticle> loadFavoritesFromFile();
};

#endif // NEWS_STORAGE_H