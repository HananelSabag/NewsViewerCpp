#include <iostream>
#include <thread>
#include <chrono>
#include "news_fetcher.h"

// API Key for the news API (replace with your actual key)
const std::string API_KEY = "9239b5e2894c476e882de87712783d9e";

// Function to display menu options
void displayMenu() {
    std::cout << "\nNews Viewer Application\n";
    std::cout << "1. View Top Headlines\n";
    std::cout << "2. Search News by Keyword\n";
    std::cout << "3. Exit\n";
    std::cout << "Choose an option (1, 2, or 3): ";
}

// Function to display news headlines
void displayNews(const std::vector<std::string>& newsList) {
    if (newsList.empty()) {
        std::cout << "[INFO] No articles found.\n";
        return;
    }

    std::cout << "\nSearch Results:\n";
    for (const auto& news : newsList) {
        std::cout << "- " << news << "\n";
    }
}

// Function to fetch headlines in a separate thread
void fetchHeadlinesAsync(NewsFetcher& newsFetcher) {
    std::cout << "[DEBUG] Fetching top headlines...\n";
    std::vector<std::string> headlines = newsFetcher.fetchHeadlines();
    displayNews(headlines);
}

// Function to handle searching news by keyword
void searchNewsAsync(NewsFetcher& newsFetcher) {
    std::string keyword;
    std::cout << " Enter a keyword to search: ";
    std::cin >> keyword;

    std::cout << "[DEBUG] Fetching news for keyword: " << keyword << "\n";
    std::vector<std::string> searchResults = newsFetcher.searchNews(keyword);
    displayNews(searchResults);
}

int main() {
    NewsFetcher newsFetcher(API_KEY);
    int choice = 0;

    while (true) {
        displayMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();  // Clear the error flag
            std::cin.ignore(10000, '\n'); // Ignore incorrect input
            std::cout << "[ERROR] Invalid input. Please enter a number (1, 2, or 3).\n";
            continue;
        }

        if (choice == 1) {
            std::thread headlinesThread(fetchHeadlinesAsync, std::ref(newsFetcher));
            headlinesThread.join();
        } else if (choice == 2) {
            std::thread searchThread(searchNewsAsync, std::ref(newsFetcher));
            searchThread.join();
        } else if (choice == 3) {
            std::cout << " Exiting the application.\n";
            break;
        } else {
            std::cout << "[ERROR] Invalid choice. Please enter 1, 2, or 3.\n";
        }
    }

    return 0;
}
