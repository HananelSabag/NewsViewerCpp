#include "news_storage.h"
#include <fstream>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

/**
* Helper function to validate article data before saving/loading
* Checks for empty titles and field length constraints
* @param article The article to validate
* @return true if article is valid, false otherwise
*/
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

/**
* Saves a list of favorite articles to a JSON file
* Validates articles before saving and logs debug info
* @param favorites Vector of articles to save
*/
void NewsStorage::saveFavoritesToFile(const std::vector<NewsFetcher::NewsArticle>& favorites) {
    // Validate articles before saving
    std::vector<NewsFetcher::NewsArticle> validFavorites;
    for (const auto& article : favorites) {
        if (isValidArticle(article)) {
            validFavorites.push_back(article);
        }
    }

    std::cout << "[DEBUG] Saving " << validFavorites.size() << " valid articles to favorites\n";

    std::ofstream outFile("favorites.json");
    if (outFile.is_open()) {
        // Convert articles to JSON array
        json favoritesJson = json::array();

        for (const auto& article : validFavorites) {
            // Create JSON object for each article
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

        // Log preview of JSON before saving
        std::string jsonStr = favoritesJson.dump(4);
        std::cout << "[DEBUG] First 200 chars of JSON to save:\n" << jsonStr.substr(0, 200) << "...\n";

        outFile << jsonStr;
        outFile.close();
        std::cout << "[INFO] Favorites saved successfully.\n";
    } else {
        std::cerr << "[ERROR] Unable to save favorites to file.\n";
    }
}

/**
* Loads favorite articles from the JSON file
* Validates articles during loading and handles errors
* @return Vector of loaded favorite articles
*/
std::vector<NewsFetcher::NewsArticle> NewsStorage::loadFavoritesFromFile() {
    std::vector<NewsFetcher::NewsArticle> favorites;
    std::ifstream inFile("favorites.json");

    if (inFile.is_open()) {
        try {
            // Read entire file into string
            std::string jsonContent((std::istreambuf_iterator<char>(inFile)),
                                    std::istreambuf_iterator<char>());

            if (jsonContent.empty()) {
                std::cout << "[INFO] Favorites file is empty\n";
                return favorites;
            }

            // Log preview of loaded JSON
            std::cout << "[DEBUG] First 200 chars of loaded JSON:\n" << jsonContent.substr(0, 200) << "...\n";

            json favoritesJson = json::parse(jsonContent);

            // Verify JSON structure
            if (!favoritesJson.is_array()) {
                throw std::runtime_error("JSON root is not an array");
            }

            // Parse each article
            for (const auto& articleJson : favoritesJson) {
                if (!articleJson.contains("title")) {
                    std::cerr << "[WARNING] Article missing title, skipping\n";
                    continue;
                }

                // Create article object with optional fields defaulting to empty string
                NewsFetcher::NewsArticle article(
                        articleJson["title"].get<std::string>(),
                        articleJson.value("description", ""),
                        articleJson.value("content", ""),
                        articleJson.value("url", ""),
                        articleJson.value("source", ""),
                        articleJson.value("publishedAt", ""),
                        articleJson.value("urlToImage", "")
                );

                // Add only valid articles
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