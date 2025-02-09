#include "ui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>
#include <filesystem>
#include <algorithm>
#include <string>

namespace fs = std::filesystem;

NewsUI::NewsUI(NewsFetcher& fetcher) : fetcher(fetcher), searchQuery("") {
    favorites = NewsStorage::loadFavoritesFromFile();
}

bool NewsUI::initializeFonts() {
    ImGuiIO& io = ImGui::GetIO();

    // Load default font
    defaultFont = io.Fonts->AddFontFromFileTTF("assets/Roboto-Regular.ttf", fontSize);
    if (!defaultFont) {
        std::cerr << "[ERROR] Failed to load default font!" << std::endl;
        return false;
    }

    // Configure Font Awesome
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = fontSize;

    // Load Font Awesome icons
    static const ImWchar fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    if (fs::exists("assets/fa-solid-900.ttf")) {
        iconFont = io.Fonts->AddFontFromFileTTF(
                "assets/fa-solid-900.ttf",
                fontSize,
                &icons_config,
                fa_ranges
        );
        if (!iconFont) {
            std::cerr << "[WARNING] Failed to load icon font!" << std::endl;
        } else {
            std::cout << "[INFO] Icon font loaded successfully." << std::endl;
        }
    } else {
        std::cerr << "[ERROR] Font Awesome font file not found!" << std::endl;
    }

    io.Fonts->Build();
    return true;
}

void NewsUI::checkFontSizeChange() {
    if (fontSizeChanged && !ImGui::GetIO().WantTextInput) {
        fontSize = pendingFontSize;
        ImGui::GetIO().Fonts->Clear();
        initializeFonts();
        ImGui_ImplOpenGL3_DestroyFontsTexture();
        ImGui_ImplOpenGL3_CreateFontsTexture();
        fontSizeChanged = false;
    }
}

void NewsUI::renderIconButton(const char* icon, const char* tooltip) {
    pushIcon();
    bool clicked = ImGui::Button(icon);
    popIcon();

    if (tooltip && ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tooltip);
    }
}


void NewsUI::applyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Define our colors
    ImVec4 primaryBlue = ImVec4(0.20f, 0.40f, 0.80f, 1.0f);
    ImVec4 primaryBlueHover = ImVec4(0.25f, 0.45f, 0.85f, 1.0f);
    ImVec4 primaryBlueActive = ImVec4(0.15f, 0.35f, 0.75f, 1.0f);

    if (isDarkMode) {
        ImGui::StyleColorsDark();
        backgroundColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        buttonColor = primaryBlue;
        buttonHoverColor = primaryBlueHover;

        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 0.50f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
    } else {
        ImGui::StyleColorsLight();
        backgroundColor = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
        textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        buttonColor = primaryBlue;
        buttonHoverColor = primaryBlueHover;

        style.Colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.50f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    }

    // Apply button colors for both themes
    style.Colors[ImGuiCol_Button] = buttonColor;
    style.Colors[ImGuiCol_ButtonHovered] = buttonHoverColor;
    style.Colors[ImGuiCol_ButtonActive] = primaryBlueActive;

    // Common style settings
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.ItemSpacing = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);

    // Improve scrollbar visibility
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 12.0f;
}

void NewsUI::run() {
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW\n";
        return;
    }

    GLFWwindow* window = glfwCreateWindow(1200, 800, "News Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "[ERROR] Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    if (!initializeFonts()) {
        std::cerr << "[ERROR] Font initialization failed!\n";
        return;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    applyTheme();
    headlines = fetcher.fetchHeadlines();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        checkFontSizeChange();  // Check for font size changes
        render();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(
                backgroundColor.x,
                backgroundColor.y,
                backgroundColor.z,
                backgroundColor.w
        );
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    NewsStorage::saveFavoritesToFile(favorites);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
void NewsUI::renderToolbar() {
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    float paddingX = 30.0f;  // Same padding as content area

    // Add right padding to toolbar
    ImGui::SetCursorPosX(paddingX);

    // Search icon and input
    pushIcon();
    ImGui::Text(FA_ICON_SEARCH);
    popIcon();

    ImGui::SameLine();
    static char searchBuffer[256] = "";
    std::strncpy(searchBuffer, searchQuery.c_str(), sizeof(searchBuffer));
    searchBuffer[sizeof(searchBuffer) - 1] = '\0';

    // Calculate search bar width (made narrower)
    float searchBarWidth = contentSize.x - (paddingX * 3) - 400.0f;  // Increased button space to 400
    ImGui::PushItemWidth(searchBarWidth);
    if (ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer),
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
        searchQuery = std::string(searchBuffer);
        handleSearch();
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::Button("Search")) {
        handleSearch();
    }

    // Position buttons on the right
    float buttonsStartX = contentSize.x - 280.0f;
    ImGui::SameLine(buttonsStartX);

    // Theme toggle button
    pushIcon();
    if (ImGui::Button(isDarkMode ? FA_ICON_SUN : FA_ICON_MOON)) {
        isDarkMode = !isDarkMode;
        applyTheme();
    }
    popIcon();
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(isDarkMode ? "Switch to Light Mode" : "Switch to Dark Mode");
    }

    // Settings button
    ImGui::SameLine();
    pushIcon();
    if (ImGui::Button(FA_ICON_COG)) {
        showSettings = true;
    }
    popIcon();
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Settings");
    }

    // Favorites button
    ImGui::SameLine();
    pushIcon();
    if (ImGui::Button(FA_ICON_STAR)) {
        showFavoritesPopup = true;
    }
    popIcon();
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Favorites");
    }

    // Home button (only shown when not on home page)
    if (!showHome) {
        ImGui::SameLine();
        pushIcon();
        if (ImGui::Button(FA_ICON_HOME)) {
            showHome = true;
            searchResults.clear();
        }
        popIcon();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Back to Home");
        }
    }
}

void NewsUI::renderNewsContent() {
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("NewsContent", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    const auto& displayList = showHome ? headlines : searchResults;

    if (displayList.empty()) {
        ImGui::TextColored(textColor, "No articles to display.");
    } else {
        for (const auto& article : displayList) {
            if (&article != &displayList.front()) {
                ImGui::Dummy(ImVec2(0.0f, 10.0f));  // הוספת מרווח בין הכותרות
            }

            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);

            std::string articleId = "Article" + std::to_string(reinterpret_cast<size_t>(&article));
            ImGui::BeginChild(articleId.c_str(), ImVec2(contentSize.x, 120), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

            bool clickedOnFavorite = false;  // משתנה שנשתמש בו לבדוק אם לחצו על הכפתור של המועדפים

            // ✅ הצגת הכותרת בצורה ברורה יותר
            ImGui::TextWrapped("%s", article.title.c_str());

            // ✅ הצגת תקציר עם תוספת מקום
            if (!article.description.empty()) {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s",
                                   article.description.substr(0, 150).c_str());
            }

            // ✅ שיפור עיצוב הכפתור של המועדפים
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
            bool isFavorite = std::find(favorites.begin(), favorites.end(), article.title) != favorites.end();

            pushIcon();
            if (isFavorite) {
                ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
                std::string btnId = FA_ICON_STAR "##" + article.title;
                if (ImGui::Button(btnId.c_str())) {
                    removeFavorite(article.title);
                    clickedOnFavorite = true;  // ✅ סימון שלחצו על הכפתור
                }
                ImGui::PopStyleColor();
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Remove from favorites");
                }
            } else {
                std::string btnId = FA_ICON_STAR_O "##" + article.title;
                if (ImGui::Button(btnId.c_str())) {
                    addToFavorites(article.title);
                    clickedOnFavorite = true;  // ✅ סימון שלחצו על הכפתור
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Add to favorites");
                }
            }
            popIcon();

            // ✅ תיקון הבעיה: פתיחת הפופאפ רק אם לא לחצו על כפתור המועדפים!
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !clickedOnFavorite) {
                selectedArticle = article;
                showArticlePopup = true;
            }

            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
    }

    ImGui::EndChild();

    if (showArticlePopup) {
        renderArticlePopup();
    }
}




void NewsUI::renderSettingsPopup() {
    if (showSettings) {
        ImGui::OpenPopup("Settings");
    }

    if (ImGui::BeginPopupModal("Settings", &showSettings)) {
        pushIcon();
        ImGui::Text(FA_ICON_COG);
        popIcon();
        ImGui::SameLine();
        ImGui::Text(" Appearance Settings");
        ImGui::Separator();

        ImGui::Text("Theme");
        if (ImGui::Checkbox("Dark Mode", &isDarkMode)) {
            applyTheme();
        }

        ImGui::Separator();
        if (ImGui::Button("Close")) {
            showSettings = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
void NewsUI::renderFavoritesPopup() {
    ImGui::OpenPopup("Favorites");


    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Favorites", &showFavoritesPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
        pushIcon();
        ImGui::Text(FA_ICON_STAR);
        popIcon();
        ImGui::SameLine();
        ImGui::Text(" Saved Articles");
        ImGui::Separator();


        ImGui::BeginChild("FavoritesList", ImVec2(850, 450), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        if (favorites.empty()) {
            ImGui::TextColored(textColor, "No favorites added yet.");
        } else {
            for (const auto& favorite : favorites) {
                ImGui::PushStyleColor(ImGuiCol_Text, textColor);


                ImGui::TextWrapped("%s", favorite.c_str());

                ImGui::SameLine();
                pushIcon();


                if (ImGui::Button((FA_ICON_TIMES "##" + favorite).c_str())) {
                    removeFavorite(favorite);
                }

                popIcon();
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Remove from favorites");
                }

                ImGui::PopStyleColor();
                ImGui::Separator();
            }
        }

        ImGui::EndChild();


        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(120, 40))) {
            showFavoritesPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void NewsUI::render() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::Begin("News Viewer", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    renderToolbar();
    ImGui::Separator();

    if (showHome) {
        pushIcon();
        ImGui::Text(FA_ICON_HOME);
        popIcon();
        ImGui::SameLine();
        ImGui::Text(" Top Headlines");
    } else {
        pushIcon();
        ImGui::Text(FA_ICON_SEARCH);
        popIcon();
        ImGui::SameLine();
        ImGui::Text(" Search Results");
    }

    renderNewsContent();

    if (showFavoritesPopup) {
        renderFavoritesPopup();
    }

    if (showSettings) {
        renderSettingsPopup();
    }

    ImGui::End();
}

void NewsUI::handleSearch() {
    if (!searchQuery.empty()) {
        searchResults = fetcher.searchNews(searchQuery);
        showHome = false;
    }
}

// Helper functions remain unchanged since they work with strings
void NewsUI::addToFavorites(const std::string& headline) {
    if (std::find(favorites.begin(), favorites.end(), headline) == favorites.end()) {
        favorites.push_back(headline);
        NewsStorage::saveFavoritesToFile(favorites);
    }
}

void NewsUI::removeFavorite(const std::string& headline) {
    favorites.erase(
            std::remove(favorites.begin(), favorites.end(), headline),
            favorites.end()
    );
    NewsStorage::saveFavoritesToFile(favorites);
}
void NewsUI::renderArticlePopup() {
    ImGui::OpenPopup("Article Details");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Article Details", &showArticlePopup,
                               ImGuiWindowFlags_AlwaysAutoResize)) {

        if (selectedArticle) {  // Check if we have a valid article
            // Title with larger font
            ImGui::PushFont(defaultFont);
            ImGui::TextWrapped("%s", selectedArticle->title.c_str());
            ImGui::PopFont();
            ImGui::Separator();

            // Source and date
            if (!selectedArticle->source.empty()) {
                ImGui::Text("Source: %s", selectedArticle->source.c_str());
                ImGui::SameLine();
            }
            if (!selectedArticle->publishedAt.empty()) {
                ImGui::Text("| Published: %s", selectedArticle->publishedAt.c_str());
            }
            ImGui::Separator();

            // Description in a scrolling region
            if (!selectedArticle->description.empty()) {
                ImGui::TextWrapped("%s", selectedArticle->description.c_str());
                ImGui::Spacing();
            }

            // Content in a scrolling region
            if (!selectedArticle->content.empty()) {
                ImGui::BeginChild("Content", ImVec2(0, 200), true);
                ImGui::TextWrapped("%s", selectedArticle->content.c_str());
                ImGui::EndChild();
            }

            ImGui::Separator();

            // URL Button
            if (!selectedArticle->url.empty()) {
                if (ImGui::Button("Open in Browser")) {
                    // Open URL in default browser - platform specific
#ifdef _WIN32
                    system(("start " + selectedArticle->url).c_str());
#elif __APPLE__
                    system(("open " + selectedArticle->url).c_str());
                    #else
                        system(("xdg-open " + selectedArticle->url).c_str());
#endif
                }
                ImGui::SameLine();
            }
        }

        if (ImGui::Button("Close")) {
            showArticlePopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}