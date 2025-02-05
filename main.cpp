#include <iostream>
#include "news_fetcher.h"

int main() {
    std::string apiKey = "9239b5e2894c476e882de87712783d9e";  // Replace with a valid key
    NewsFetcher newsFetcher(apiKey);

    std::vector<std::string> headlines = newsFetcher.fetchHeadlines();

    std::cout << "\nTop News Headlines:\n";
    if (headlines.empty()) {
        std::cout << "[WARNING] No headlines found!" << std::endl;
    }

    for (const auto& headline : headlines) {
        std::cout << "- " << headline << std::endl;
    }

    return 0;
}
