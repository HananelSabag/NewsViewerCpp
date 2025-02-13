#include "news_fetcher.h"
#include "ui.h"
#include <iostream>
#include <chrono>
#include <ctime>

int main() {
    try {
        // Start time logging
        auto start = std::chrono::system_clock::now();
        std::time_t start_time = std::chrono::system_clock::to_time_t(start);
        std::cout << "[INFO] Application starting at: " << std::ctime(&start_time);

        // Initialize NewsAPI
        std::string apiKey = "9239b5e2894c476e882de87712783d9e";
        std::cout << "[DEBUG] Initializing NewsAPI with key length: " << apiKey.length() << std::endl;
        NewsFetcher newsFetcher(apiKey);

        // Initialize UI
        std::cout << "[INFO] Initializing UI components..." << std::endl;
        NewsUI ui(newsFetcher);

        // Start auto-update process (5 minutes interval)
        std::cout << "[INFO] Starting auto-update service with 5 minute interval" << std::endl;
        newsFetcher.startAutoUpdate(10);

        // Run main application
        std::cout << "[INFO] Starting main application loop..." << std::endl;
        ui.run();

        // Clean shutdown
        std::cout << "[INFO] Application shutting down, stopping auto-update..." << std::endl;
        newsFetcher.stopAutoUpdate();

        // Log total runtime
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "[INFO] Total runtime: " << elapsed.count() << " seconds" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] Application error: " << e.what() << std::endl;
        return 1;
    }
}