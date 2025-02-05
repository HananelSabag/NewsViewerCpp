
#include "news_fetcher.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using nlohmann::json;

// Constructor - Initializes the API key
NewsFetcher::NewsFetcher(const string& apiKey) : apiKey(apiKey) {}

// Makes the HTTP request to NewsAPI and returns the response as a string
string NewsFetcher::makeRequest() {
    httplib::SSLClient cli("newsapi.org");  // Use SSL for HTTPS
    string requestUrl = "/v2/top-headlines?country=us&apiKey=" + apiKey;

    cout << "[DEBUG] Sending request to: " << requestUrl << endl;

    auto res = cli.Get(requestUrl.c_str());

    if (!res) {
        cerr << "[ERROR] Connection failed. Ensure OpenSSL is installed correctly." << endl;
        return "";
    }

    cout << "[DEBUG] HTTP Status Code: " << res->status << endl;

    if (res->status == 200) {
        cout << "[DEBUG] Successfully fetched news data." << endl;
        cout << "[DEBUG] Response Body: " << res->body.substr(0, 500) << "..." << endl;
        return res->body;
    }
    else {
        cerr << "[ERROR] HTTP Error: " << res->status << " - " << res->body << endl;
    }

    return "";
}

// Parses JSON and extracts news headlines
vector<string> NewsFetcher::fetchHeadlines() {
    vector<string> headlines;
    string response = makeRequest();

    if (response.empty()) {
        cerr << "[ERROR] No data received from NewsAPI." << endl;
        return headlines;
    }

    try {
        json jsonData = json::parse(response);
        cout << "[DEBUG] Parsed JSON Data:\n" << jsonData.dump(4) << endl;

        if (jsonData.contains("articles") && jsonData["articles"].is_array()) {
            for (const auto& article : jsonData["articles"]) {
                if (article.contains("title") && article["title"].is_string()) {
                    string title = article["title"];
                    cout << "[INFO] Extracted Title: " << title << endl;
                    headlines.push_back(title);
                }
            }
        }
        else {
            cerr << "[ERROR] JSON response does not contain 'articles' array." << endl;
        }
    }
    catch (const exception& e) {
        cerr << "[ERROR] JSON parsing failed: " << e.what() << endl;
    }

    return headlines;
}
