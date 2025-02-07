#ifndef NEWS_STORAGE_H
#define NEWS_STORAGE_H

#include <vector>
#include <string>

class NewsStorage {
public:
    static void saveFavoritesToFile(const std::vector<std::string>& favorites);
    static std::vector<std::string> loadFavoritesFromFile();
};

#endif // NEWS_STORAGE_H
