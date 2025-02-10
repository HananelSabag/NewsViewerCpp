#include "news_storage.h"
#include <fstream>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

// Helper function to validate article data
static bool isValidArticle(const NewsFetcher::NewsArticle& article) {
    if (article.title.empty()) {
        std::cerr << "[WARNING] Article has empty title\n";
        return false;
    }

    if (article.title.length() > 500 ||
        article.description.length() > 2000 ||
        article.content.length() > 10000) {
        std::cerr << "[WARNING] Article fields exceed maximum length\n";
        return false;
    }

    return true;
}

void NewsStorage::saveFavoritesToFile(const std::vector<NewsFetcher::NewsArticle>& favorites) {
    // First validate all articles
    std::vector<NewsFetcher::NewsArticle> validFavorites;
    for (const auto& article : favorites) {
        if (isValidArticle(article)) {
            validFavorites.push_back(article);
        }
    }

    std::cout << "[DEBUG] Saving " << validFavorites.size() << " valid articles to favorites\n";

    std::ofstream outFile("favorites.json");
    if (outFile.is_open()) {
        json favoritesJson = json::array();

        for (const auto& article : validFavorites) {
            json articleJson = {
                    {"title", article.title},
                    {"description", article.description},
                    {"content", article.content},
                    {"url", article.url},
                    {"source", article.source},
                    {"publishedAt", article.publishedAt},
                    {"urlToImage", article.urlToImage}
            };
            favoritesJson.push_back(articleJson);
        }

        std::string jsonStr = favoritesJson.dump(4);
        std::cout << "[DEBUG] First 200 chars of JSON to save:\n" << jsonStr.substr(0, 200) << "...\n";

        outFile << jsonStr;
        outFile.close();
        std::cout << "[INFO] Favorites saved successfully.\n";
    } else {
        std::cerr << "[ERROR] Unable to save favorites to file.\n";
    }
}

std::vector<NewsFetcher::NewsArticle> NewsStorage::loadFavoritesFromFile() {
    std::vector<NewsFetcher::NewsArticle> favorites;
    std::ifstream inFile("favorites.json");

    if (inFile.is_open()) {
        try {
            std::string jsonContent((std::istreambuf_iterator<char>(inFile)),
                                    std::istreambuf_iterator<char>());

            if (jsonContent.empty()) {
                std::cout << "[INFO] Favorites file is empty\n";
                return favorites;
            }

            std::cout << "[DEBUG] First 200 chars of loaded JSON:\n" << jsonContent.substr(0, 200) << "...\n";

            json favoritesJson = json::parse(jsonContent);

            if (!favoritesJson.is_array()) {
                throw std::runtime_error("JSON root is not an array");
            }

            for (const auto& articleJson : favoritesJson) {
                if (!articleJson.contains("title")) {
                    std::cerr << "[WARNING] Article missing title, skipping\n";
                    continue;
                }

                NewsFetcher::NewsArticle article(
                        articleJson["title"].get<std::string>(),
                        articleJson.value("description", ""),
                        articleJson.value("content", ""),
                        articleJson.value("url", ""),
                        articleJson.value("source", ""),
                        articleJson.value("publishedAt", ""),
                        articleJson.value("urlToImage", "")
                );

                if (isValidArticle(article)) {
                    favorites.push_back(article);
                }
            }

            std::cout << "[INFO] Successfully loaded " << favorites.size() << " articles from favorites\n";

        } catch (const json::parse_error& e) {
            std::cerr << "[ERROR] JSON parse error: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Error loading favorites: " << e.what() << "\n";
        }
        inFile.close();
    } else {
        std::cout << "[INFO] No favorites file found, starting fresh.\n";
    }

    return favorites;
}