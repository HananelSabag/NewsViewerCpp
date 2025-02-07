#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>

/**
 * @brief Constructor - Initializes the UI with a reference to a NewsFetcher instance.
 */
NewsUI::NewsUI(NewsFetcher& fetcher) : fetcher(fetcher), searchQuery("") {
    favorites = NewsStorage::loadFavoritesFromFile();  // ✅ Load favorites from file
}

/**
 * @brief Runs the main UI loop.
 */
void NewsUI::run() {
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW\n";
        return;
    }

    // Create window with appropriate size
    GLFWwindow* window = glfwCreateWindow(1000, 700, "News Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "[ERROR] Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault(); // Load default font

    // Load Noto Emoji Font with a safe Unicode range
    ImFontConfig config;
    config.MergeMode = true;  // Merge emoji font with default font
    config.PixelSnapH = true;

    // ✅ Using a valid ImWchar array (16-bit characters only)
    static const ImWchar emojiRanges[] = { 0x2600, 0x26FF,  0 };  // Weather & Faces Emojis

    ImFont* defaultFont = io.Fonts->AddFontDefault();  // Load default font

    ImFont* emojiFont = io.Fonts->AddFontFromFileTTF("assets/NotoEmoji-Regular.ttf", 18.0f, &config, emojiRanges);

    if (!emojiFont) {
        std::cerr << "[ERROR] Failed to load emoji font!" << std::endl;
    } else {
        std::cout << "[INFO] Emoji font loaded successfully." << std::endl;
    }

    // Upload font textures to GPU
    io.Fonts->Build();


    ImGui::StyleColorsLight();  // ✅ Set UI to a brighter theme

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Fetch initial headlines
    headlines = fetcher.fetchHeadlines();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        render();

        ImGui::Render();
        glViewport(0, 0, 1000, 700);
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);  // ✅ Light gray background
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    NewsStorage::saveFavoritesToFile(favorites); // ✅ Save favorites before exiting
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

/**
 * @brief Renders the ImGui interface.
 */
void NewsUI::render() {
    ImGui::Begin("News Viewer", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Search Bar
    static char searchBuffer[256] = "";
    std::strncpy(searchBuffer, searchQuery.c_str(), sizeof(searchBuffer));
    searchBuffer[sizeof(searchBuffer) - 1] = '\0';

    ImGui::Text("🔎 Search for News:");
    if (ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer))) {
        searchQuery = std::string(searchBuffer);
    }

    ImGui::SameLine();
    if (ImGui::Button("Search")) {
        handleSearch();
    }

    ImGui::SameLine();
    if (ImGui::Button("⭐ Favorites")) {
        showFavoritesPopup = true;
    }

    if (!showHome) {
        ImGui::SameLine();
        if (ImGui::Button("🏠 Back to Home")) {
            showHome = true;
            searchResults.clear();
        }
    }

    ImGui::Separator();

    // Display Top Headlines or Search Results
    if (showHome) {
        ImGui::Text(u8"\U0001F4F0 Top Headlines:");
        ImGui::BeginChild("Headlines", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        for (const auto& headline : headlines) {
            ImGui::BulletText("%s", headline.c_str());
            ImGui::SameLine();
            if (ImGui::Button(("⭐##" + headline).c_str())) {
                addToFavorites(headline);
            }
        }
        ImGui::EndChild();
    } else {
        ImGui::Text("🔍 Search Results:");
        ImGui::BeginChild("SearchResults", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        for (const auto& result : searchResults) {
            ImGui::BulletText("%s", result.c_str());
            ImGui::SameLine();
            if (ImGui::Button(("⭐##" + result).c_str())) {
                addToFavorites(result);
            }
        }
        ImGui::EndChild();
    }

    // Handle Favorites Popup
    if (showFavoritesPopup) {
        renderFavoritesPopup();
    }

    ImGui::End();
}

/**
 * @brief Displays the favorites list in a popup window.
 */
void NewsUI::renderFavoritesPopup() {
    ImGui::OpenPopup("Favorites");
    if (ImGui::BeginPopupModal("Favorites", &showFavoritesPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("⭐ Saved Favorite Articles:");
        ImGui::Separator();

        if (favorites.empty()) {
            ImGui::Text("No favorites added yet.");
        } else {
            for (size_t i = 0; i < favorites.size(); ++i) {
                ImGui::BulletText("%s", favorites[i].c_str());
                ImGui::SameLine();
                std::string buttonLabel = "❌##" + std::to_string(i);
                if (ImGui::Button(buttonLabel.c_str())) {
                    removeFavorite(favorites[i]);
                }
            }
        }

        if (ImGui::Button("Close")) {
            showFavoritesPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

/**
 * @brief Handles user input and searches for news.
 */
void NewsUI::handleSearch() {
    if (!searchQuery.empty()) {
        searchResults = fetcher.searchNews(searchQuery);
        showHome = false;  // ✅ Switch to search results view
    }
}

/**
 * @brief Adds a news headline to favorites and saves it to file.
 */
void NewsUI::addToFavorites(const std::string& headline) {
    if (std::find(favorites.begin(), favorites.end(), headline) == favorites.end()) {
        favorites.push_back(headline);
        NewsStorage::saveFavoritesToFile(favorites);
    }
}

/**
 * @brief Removes a news headline from favorites and updates the file.
 */
void NewsUI::removeFavorite(const std::string& headline) {
    favorites.erase(std::remove(favorites.begin(), favorites.end(), headline), favorites.end());
    NewsStorage::saveFavoritesToFile(favorites);
}
