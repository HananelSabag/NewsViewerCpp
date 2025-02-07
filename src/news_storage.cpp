#include "news_storage.h"
#include <fstream>
#include <iostream>

/**
 * @brief Saves favorite headlines to a file.
 */
void NewsStorage::saveFavoritesToFile(const std::vector<std::string>& favorites) {
    std::ofstream outFile("favorites.txt");
    if (outFile.is_open()) {
        for (const auto& fav : favorites) {
            outFile << fav << std::endl;
        }
        outFile.close();
    } else {
        std::cerr << "[ERROR] Unable to save favorites to file.\n";
    }
}

/**
 * @brief Loads favorite headlines from a file.
 */
std::vector<std::string> NewsStorage::loadFavoritesFromFile() {
    std::vector<std::string> favorites;
    std::ifstream inFile("favorites.txt");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            favorites.push_back(line);
        }
        inFile.close();
    } else {
        std::cerr << "[WARNING] No favorites file found, starting fresh.\n";
    }
    return favorites;
}
