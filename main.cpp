#include "news_fetcher.h"
#include "ui.h"
#include <iostream>

int main() {
    // Define API key (replace with your own API key)
    std::string apiKey = "9239b5e2894c476e882de87712783d9e";

    // Create NewsFetcher instance
    NewsFetcher newsFetcher(apiKey);
    newsFetcher.startAutoUpdate(10);

    // Create and run the UI
    NewsUI ui(newsFetcher);
    ui.run();
    newsFetcher.stopAutoUpdate();

    return 0;
}
