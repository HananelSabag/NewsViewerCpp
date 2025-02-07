#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>  // Required for strcpy

/**
 * @brief Constructor - Initializes the UI with a reference to a NewsFetcher instance.
 */
NewsUI::NewsUI(NewsFetcher& fetcher) : fetcher(fetcher), searchQuery("") {}

/**
 * @brief Runs the main UI loop.
 */
void NewsUI::run() {
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW\n";
        return;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "News Viewer", nullptr, nullptr);
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
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); // Set UI style

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
        glViewport(0, 0, 800, 600);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
}

/**
 * @brief Renders the ImGui interface.
 */
void NewsUI::render() {
    ImGui::Begin("News Viewer");

    // Search Bar
    static char searchBuffer[256] = "";  // Buffer to store user input
    std::strncpy(searchBuffer, searchQuery.c_str(), sizeof(searchBuffer));
    searchBuffer[sizeof(searchBuffer) - 1] = '\0';

    ImGui::Text("Search for News:");
    if (ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer))) {
        searchQuery = std::string(searchBuffer);
    }

    if (ImGui::Button("Search")) {
        handleSearch();
    }

    ImGui::Separator();

    // Display Headlines
    ImGui::Text("Top Headlines:");
    for (const auto& headline : headlines) {
        ImGui::BulletText("%s", headline.c_str());
    }

    // Display Search Results
    if (!searchResults.empty()) {
        ImGui::Separator();
        ImGui::Text("Search Results:");
        for (const auto& result : searchResults) {
            ImGui::BulletText("%s", result.c_str());
        }
    }

    ImGui::End();
}

/**
 * @brief Handles user input and searches for news.
 */
void NewsUI::handleSearch() {
    if (!searchQuery.empty()) {
        searchResults = fetcher.searchNews(searchQuery);
    }
}

/**
 * @brief Cleans up ImGui resources.
 */
void NewsUI::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
