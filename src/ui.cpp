    #include "ui.h"
    #include "imgui_impl_glfw.h"
    #include "imgui_impl_opengl3.h"
    #include "imgui_internal.h"

    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"

    #include <iostream>
    #include <filesystem>
    namespace fs = std::filesystem;

    NewsUI::NewsUI(NewsFetcher& fetcher) : fetcher(fetcher) {

        favorites = NewsStorage::loadFavoritesFromFile();

        // Initialize state with default values
        state.searchQuery = "";
        state.isDarkMode = true;
    }
    // Destructor ensures proper cleanup of OpenGL resources
    NewsUI::~NewsUI() {
        for (auto& [url, texture] : textureCache) {
            deleteTexture(texture);
        }
        textureCache.clear();
    }

    // Initialize fonts and build font atlas
    bool NewsUI::initializeFonts() {
        ImGuiIO& io = ImGui::GetIO();

        // Load default font
        std::string robotoPath = "assets/Roboto-Regular.ttf";
        if (!fs::exists(robotoPath)) {
            std::cerr << "[ERROR] Default font not found: " << robotoPath << std::endl;
            return false;
        }

        defaultFont = io.Fonts->AddFontFromFileTTF(robotoPath.c_str(), DEFAULT_FONT_SIZE);
        if (!defaultFont) {
            std::cerr << "[ERROR] Failed to load default font" << std::endl;
            return false;
        }

        // Configure and load icon font
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.GlyphMinAdvanceX = DEFAULT_FONT_SIZE;

        static const ImWchar fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        std::string iconPath = "assets/fa-solid-900.ttf";

        if (!fs::exists(iconPath)) {
            std::cerr << "[WARNING] Icon font not found: " << iconPath << std::endl;
            return true; // Continue without icons
        }

        iconFont = io.Fonts->AddFontFromFileTTF(iconPath.c_str(),
                                                DEFAULT_FONT_SIZE,
                                                &icons_config,
                                                fa_ranges);

        if (!iconFont) {
            std::cerr << "[WARNING] Failed to load icon font" << std::endl;
            return true; // Continue without icons
        }

        io.Fonts->Build();
        return true;
    }



    // Setup GLFW window with proper settings
    void NewsUI::setupWindow(GLFWwindow* window) {
        // Set window size to 80% of primary monitor resolution
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primary);

        int width = static_cast<int>(mode->width * 0.8);
        int height = static_cast<int>(mode->height * 0.8);

        glfwSetWindowSize(window, width, height);

        // Center window on screen
        int xpos = (mode->width - width) / 2;
        int ypos = (mode->height - height) / 2;
        glfwSetWindowPos(window, xpos, ypos);

        // Initialize OpenGL context
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync
    }

    // Main UI run loop
    void NewsUI::run() {
        if (!glfwInit()) {
            std::cerr << "[ERROR] Failed to initialize GLFW" << std::endl;
            return;
        }

        // Create window with initial size
        GLFWwindow* window = glfwCreateWindow(1200, 800, "News Viewer", nullptr, nullptr);
        if (!window) {
            std::cerr << "[ERROR] Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        setupWindow(window);



        // Initialize Dear ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // Initialize fonts
        if (!initializeFonts()) {
            cleanup(window);
            return;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        // Apply initial theme
        applyTheme();

        // Set custom icon
        setWindowIcon(window, "assets/app_icon.png");

        // Fetch initial headlines
        try {
            headlines = fetcher.fetchHeadlines();
            showMessage("Headlines loaded successfully", colors.success);
        } catch (const std::exception& e) {
            showMessage("Failed to load headlines", colors.error);
            std::cerr << "[ERROR] Failed to fetch headlines: " << e.what() << std::endl;
        }

        // Main loop
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // Start new frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();


            // Update and render messages
            updateMessages();

            // Render main UI
            render();

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);

            glClearColor(
                    colors.background.x,
                    colors.background.y,
                    colors.background.z,
                    colors.background.w
            );
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }

        // Save favorites before exit
        NewsStorage::saveFavoritesToFile(favorites);
        cleanup(window);
    }

     //Sets a custom icon for the GLFW window.
    // window The GLFW window where the icon will be applied.
    //iconPath Path to the icon image (must be PNG format).

    void NewsUI::setWindowIcon(GLFWwindow* window, const char* iconPath) {
        std::cout << "[DEBUG] setWindowIcon() was called. Attempting to load: " << iconPath << std::endl;

        GLFWimage icon;
        int width, height, channels;

        // Check if the file exists
        if (!fs::exists(iconPath)) {
            std::cerr << "[ERROR] Icon file not found at: " << iconPath << std::endl;
            return;
        }

        // Load the image using stb_image
        unsigned char* pixels = stbi_load(iconPath, &width, &height, &channels, 4);
        if (!pixels) {
            std::cerr << "[ERROR] Failed to load icon: " << iconPath << std::endl;
            std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
            return;
        }

        // Populate the GLFWimage structure
        icon.width = width;
        icon.height = height;
        icon.pixels = pixels;

        // Apply the icon to the GLFW window
        glfwSetWindowIcon(window, 1, &icon);

        // Free the image data
        stbi_image_free(pixels);

        std::cout << "[INFO] Icon applied successfully! (" << width << "x" << height << ")" << std::endl;
    }




    // Cleanup resources
    void NewsUI::cleanup(GLFWwindow* window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    }


    // Message system implementation
    void NewsUI::showMessage(const std::string& text, const ImVec4& color, float duration) {
        messages.push_back({text, color, duration, ImGui::GetTime()});
    }

    void NewsUI::updateMessages() {
        float currentTime = ImGui::GetTime();
        messages.erase(
                std::remove_if(messages.begin(), messages.end(),
                               [currentTime](const Message& msg) {
                                   return (currentTime - msg.startTime) > msg.duration;
                               }),
                messages.end()
        );
    }
    // Theme application
    void NewsUI::applyTheme() {
        ImGuiStyle& style = ImGui::GetStyle();

        // Define theme colors
        if (state.isDarkMode) {
            colors.background = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
            colors.text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            ImGui::StyleColorsDark();

            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
            style.Colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 0.50f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
        } else {
            colors.background = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
            colors.text = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            ImGui::StyleColorsLight();

            style.Colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.50f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
        }

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
        style.ScrollbarSize = 14.0f;
    }

    // Main render function
    void NewsUI::render() {
        // Setup main window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        ImGui::Begin("NewsViewer", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

        renderToolbar();
        ImGui::Separator();

        // Header text based on current view
        if (state.showHome) {
            pushIcon();
            ImGui::Text(ICON_HOME);
            popIcon();
            ImGui::SameLine();
            ImGui::Text(" Top Headlines");
            // Right side - article count

        } else {
            pushIcon();
            ImGui::Text(ICON_SEARCH);
            popIcon();
            ImGui::SameLine();
            ImGui::Text(" Search Results");
        }

        renderNewsContent();
        renderFooter();
        renderMessages();

        // Handle popups
        if (state.showSettings) renderSettingsPopup();
        if (state.showFavoritesPopup) renderFavoritesPopup();
        if (state.showArticlePopup) renderArticlePopup();

        ImGui::End();
    }

    // Toolbar implementation with improved search handling and consistent button behavior
    void NewsUI::renderToolbar() {
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        float paddingX = 30.0f;
        ImGui::SetCursorPosX(paddingX);

        // Search bar
        pushIcon();
        ImGui::Text(ICON_SEARCH);
        popIcon();
        ImGui::SameLine();

        static char searchBuffer[256] = "";
        std::strncpy(searchBuffer, state.searchQuery.c_str(), sizeof(searchBuffer) - 1);

        float searchBarWidth = contentSize.x - (paddingX * 3) - 400.0f;
        ImGui::PushItemWidth(searchBarWidth);

        // Handle both Enter key and button click for search
        bool enterPressed = ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer),
                                             ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        bool searchClicked = ImGui::Button("Search");

        // Perform search if either Enter was pressed or Search button was clicked
        if (enterPressed || searchClicked) {
            state.searchQuery = searchBuffer;
            handleSearch();
        }

        // Right-aligned buttons
        float buttonsStartX = contentSize.x - 330.0f;  // Increased to make room for refresh button
        ImGui::SameLine(buttonsStartX);

        // Theme toggle
        pushIcon();
        if (ImGui::Button(state.isDarkMode ? ICON_SUN : ICON_MOON)) {
            state.isDarkMode = !state.isDarkMode;
            applyTheme();
            showMessage("Theme changed", colors.success);
        }
        popIcon();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(state.isDarkMode ? "Switch to Light Mode" : "Switch to Dark Mode");
        }

        // Settings button
        ImGui::SameLine();
        pushIcon();
        renderIconButton(ICON_COG, "Settings");
        popIcon();
        if (ImGui::IsItemClicked()) {
            state.showSettings = true;
        }

        // Refresh button
        ImGui::SameLine();
        pushIcon();
        if (state.showHome) {  // Only show refresh on home page
            if (ImGui::Button(ICON_REFRESH)) {
                try {
                    headlines = fetcher.fetchHeadlines();
                    showMessage("Headlines refreshed", colors.success);
                } catch (const std::exception& e) {
                    showMessage("Failed to refresh headlines", colors.error);
                    std::cerr << "[ERROR] Refresh failed: " << e.what() << std::endl;
                }
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Click to check for new headlines\nAuto-updates every 5 minutes");
            }
        }
        popIcon();

        // Favorites button
        ImGui::SameLine();
        pushIcon();
        renderIconButton(ICON_STAR, "Favorites", &colors.warning);
        popIcon();
        if (ImGui::IsItemClicked()) {
            state.showFavoritesPopup = true;
        }

        // Home button (when not on home page)
        if (!state.showHome) {
            ImGui::SameLine();
            pushIcon();
            if (ImGui::Button(ICON_HOME)) {
                state.showHome = true;
                searchResults.clear();
                showMessage("Returned to home", colors.success);
            }
            popIcon();
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Back to Home");
            }
        }
    }

    // News content rendering
    void NewsUI::renderNewsContent() {
        ImGui::BeginChild("NewsContent",
                          ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), // Leave space for footer
                          true,
                          ImGuiWindowFlags_AlwaysVerticalScrollbar);

        const auto& displayList = state.showHome ? headlines : searchResults;

        if (displayList.empty()) {
            centerText("No articles to display.",
                       ImGui::GetContentRegionAvail().x,
                       ImGui::GetContentRegionAvail().y);
        } else {
            ImGui::Text("Articles: %zu", displayList.size());
            for (const auto& article : displayList) {
                ImGui::PushStyleColor(ImGuiCol_Text, colors.text);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);

                std::string articleId = "Article_" + std::to_string(reinterpret_cast<size_t>(&article));
                ImGui::BeginChild(articleId.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 120), true);

                bool clickedOnFavorite = false;

                // Article title with text wrapping
                float titleWidth = ImGui::GetContentRegionAvail().x - 40; // Space for favorite button
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + titleWidth);
                ImGui::TextWrapped("%s", article.title.c_str());
                ImGui::PopTextWrapPos();

                // Description
                if (!article.description.empty()) {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s",
                                       article.description.substr(0, 150).c_str());
                }

                // Favorite button
                ImGui::SetCursorPos(ImVec2(
                        ImGui::GetWindowWidth() - 40,
                        ImGui::GetCursorPosY() + 5
                ));

                bool isFavorite = std::find_if(favorites.begin(), favorites.end(),
                                               [&](const NewsFetcher::NewsArticle& fav) {
                                                   return fav.title == article.title;
                                               }) != favorites.end();

                pushIcon();
                if (isFavorite) {
                    ImGui::PushStyleColor(ImGuiCol_Button, colors.warning);
                    if (ImGui::Button((ICON_STAR "##" + article.title).c_str())) {
                        removeFavorite(article.title);
                        clickedOnFavorite = true;
                    }
                    ImGui::PopStyleColor();
                } else {
                    if (ImGui::Button((ICON_STAR_O "##" + article.title).c_str())) {
                        addToFavorites(article);
                        clickedOnFavorite = true;
                    }
                }
                popIcon();

                // Handle article click
                if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !clickedOnFavorite) {
                    selectedArticle = article;
                    state.showArticlePopup = true;
                }

                ImGui::EndChild();
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
                ImGui::Spacing();
            }
        }

        ImGui::EndChild();
    }

    // Footer implementation
    void NewsUI::renderFooter() {
        ImGui::Separator();
        ImGui::BeginChild("Footer", ImVec2(0, ImGui::GetFrameHeightWithSpacing()), false);

        // Left side - version
        ImGui::Text("Version: %s", APP_VERSION);
        // Right side - article count
        ImGui::SameLine(ImGui::GetWindowWidth() - 280);
        ImGui::Text("Created by Hananel Sabag & Amit Cohen");

        ImGui::EndChild();
    }

    // Message rendering - Updated to appear at the top of the application window
    void NewsUI::renderMessages() {
        const float PADDING = 10.0f;
        float yOffset = 0.0f; // Start positioning from the top

        for (const auto& message : messages) {
            ImVec2 textSize = ImGui::CalcTextSize(message.text.c_str());
            yOffset += textSize.y + PADDING;

            // Set the message position at the top of the window
            ImGui::SetNextWindowPos(ImVec2(
                    (ImGui::GetIO().DisplaySize.x - textSize.x) * 0.5f, // Center horizontally
                    yOffset  // Stacks messages at the top
            ));

            ImGui::SetNextWindowBgAlpha(0.85f); // Slight transparency for better visibility
            ImGui::Begin(("##Message" + message.text).c_str(), nullptr,
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoInputs |
                         ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoScrollbar
            );

            ImGui::TextColored(message.color, "%s", message.text.c_str());
            ImGui::End();
        }
    }


    // Utility functions
    void NewsUI::centerText(const char* text, float width, float height) {
        ImVec2 textSize = ImGui::CalcTextSize(text);
        ImGui::SetCursorPos(ImVec2(
                (width - textSize.x) * 0.5f,
                (height - textSize.y) * 0.5f
        ));
        ImGui::TextUnformatted(text);
    }

    void NewsUI::pushIcon() {
        if (iconFont) ImGui::PushFont(iconFont);
    }

    void NewsUI::popIcon() {
        if (iconFont) ImGui::PopFont();
    }

    void NewsUI::renderIconButton(const char* icon, const char* tooltip, const ImVec4* color) {
        if (color) ImGui::PushStyleColor(ImGuiCol_Button, *color);
        bool clicked = ImGui::Button(icon);
        if (color) ImGui::PopStyleColor();

        if (tooltip && ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tooltip);
        }
    }
    // Image texture creation from raw data
    NewsUI::ImageTexture NewsUI::createTextureFromImageData(
            const std::vector<unsigned char>& imageData) {
        ImageTexture texture;

        int channels;
        unsigned char* data = stbi_load_from_memory(
                imageData.data(),
                static_cast<int>(imageData.size()),
                &texture.width,
                &texture.height,
                &channels,
                STBI_rgb_alpha
        );

        if (!data) {
            std::cerr << "[ERROR] Failed to decode image: " << stbi_failure_reason() << std::endl;
            return texture;
        }

        glGenTextures(1, &texture.textureId);
        glBindTexture(GL_TEXTURE_2D, texture.textureId);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     texture.width, texture.height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
        texture.isLoaded = true;
        return texture;
    }

    void NewsUI::deleteTexture(ImageTexture& texture) {
        if (texture.textureId != 0) {
            glDeleteTextures(1, &texture.textureId);
            texture.textureId = 0;
            texture.width = 0;
            texture.height = 0;
            texture.isLoaded = false;
        }
    }


    void NewsUI::renderImage(NewsFetcher::NewsArticle& article, float maxWidth, float maxHeight) {
        if (article.urlToImage.empty()) {
            centerText("No image available", maxWidth, maxHeight);
            return;
        }

        // Start loading if not already started
        if (!article.isImageLoaded && !article.isImageLoading && !article.loadError) {
            fetcher.loadArticleImageOnDemand(article);
            article.isImageLoading = true;
        }

        if (article.isImageLoading) {
            static float loadingTimer = 0.0f;
            loadingTimer += ImGui::GetIO().DeltaTime;
            const char* dots = ".";
            if (fmod(loadingTimer, 3.0f) > 2.0f) dots = "...";
            else if (fmod(loadingTimer, 3.0f) > 1.0f) dots = "..";

            std::string loadingText = "Loading image" + std::string(dots);
            centerText(loadingText.c_str(), maxWidth, maxHeight);
            return;
        }

        if (article.loadError) {
            ImGui::PushStyleColor(ImGuiCol_Text, colors.error);
            centerText("Failed to load image", maxWidth, maxHeight);
            ImGui::PopStyleColor();

            // Add retry button
            float buttonWidth = 120.0f;
            ImGui::SetCursorPosX((maxWidth - buttonWidth) * 0.5f);
            if (ImGui::Button("Retry", ImVec2(buttonWidth, 0))) {
                article.loadError = false;
                article.isImageLoaded = false;
                article.isImageLoading = false;
                fetcher.loadArticleImageOnDemand(article);
            }
            return;
        }

        if (article.imageData.empty()) {
            centerText("Image load failed", maxWidth, maxHeight);
            return;
        }

        auto& texture = textureCache[article.urlToImage];
        if (!texture.isLoaded) {
            texture = createTextureFromImageData(article.imageData);
        }

        if (texture.isLoaded) {
            float aspectRatio = static_cast<float>(texture.width) / texture.height;
            float displayHeight = maxHeight;
            float displayWidth = displayHeight * aspectRatio;

            if (displayWidth > maxWidth) {
                displayWidth = maxWidth;
                displayHeight = displayWidth / aspectRatio;
            }

            float xPos = (maxWidth - displayWidth) * 0.5f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xPos);
            ImGui::Image((ImTextureID)(uint64_t)texture.textureId,
                         ImVec2(displayWidth, displayHeight));
        }
    }

    // Unified article popup styling
    void NewsUI::renderArticlePopup() {
        ImGui::OpenPopup("Article Details");

        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        ImVec2 popupSize(viewportSize.x * 0.85f, viewportSize.y * 0.85f);
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(popupSize, ImGuiCond_Appearing);

        if (ImGui::BeginPopupModal("Article Details", &state.showArticlePopup)) {
            if (selectedArticle) {
                // Source and publish date in header
                if (!selectedArticle->source.empty() || !selectedArticle->publishedAt.empty()) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));  // Light blue for header
                    if (!selectedArticle->source.empty()) {
                        ImGui::Text("Source: %s", selectedArticle->source.c_str());
                    }
                    if (!selectedArticle->publishedAt.empty()) {
                        ImGui::SameLine();
                        ImGui::Text("| Published: %s", selectedArticle->publishedAt.c_str());
                    }
                    ImGui::PopStyleColor();
                    ImGui::Separator();
                }

                // Title with blue color and larger font
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
                ImGui::SetWindowFontScale(1.5f);
                ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
                ImGui::TextWrapped("%s", selectedArticle->title.c_str());
                ImGui::PopTextWrapPos();
                ImGui::SetWindowFontScale(1.0f);
                ImGui::PopStyleColor();

                ImGui::Separator();
                ImGui::Spacing();

                // Content area
                const float footerHeight = ImGui::GetFrameHeightWithSpacing() * 3;
                ImGui::BeginChild("ArticleContent", ImVec2(0, -footerHeight), true);

                // Image section if available
                if (!selectedArticle->urlToImage.empty()) {
                    ImGui::BeginChild("ImageFrame", ImVec2(0, popupSize.y * 0.4f), true);
                    renderImage(*selectedArticle,
                                ImGui::GetContentRegionAvail().x,
                                ImGui::GetContentRegionAvail().y);
                    ImGui::EndChild();
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                }

                // Content section with better handling of missing content
                if (!selectedArticle->description.empty() || !selectedArticle->content.empty()) {
                    // Show description if available
                    if (!selectedArticle->description.empty()) {
                        ImGui::TextWrapped("%s", selectedArticle->description.c_str());
                        ImGui::Spacing();
                    }

                    // Show full content if available
                    if (!selectedArticle->content.empty()) {
                        if (!selectedArticle->description.empty()) {
                            ImGui::Separator();
                            ImGui::Spacing();
                        }
                        ImGui::TextWrapped("%s", selectedArticle->content.c_str());
                    }
                } else {
                    // Show message when no content is available
                    ImGui::Spacing();
                    ImGui::PushStyleColor(ImGuiCol_Text, colors.warning);
                    centerText("Full article content is not available in the preview.",
                               ImGui::GetContentRegionAvail().x,
                               ImGui::GetContentRegionAvail().y * 0.3f);

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
                    centerText("Please use 'Open in Browser' to read the complete article.",
                               ImGui::GetContentRegionAvail().x,
                               ImGui::GetContentRegionAvail().y * 0.3f);
                    ImGui::PopStyleColor();
                }

                ImGui::EndChild();
                ImGui::Separator();

                // Footer buttons
                float buttonWidth = 150.0f;
                float totalButtonWidth = buttonWidth * 3 + ImGui::GetStyle().ItemSpacing.x * 2;
                float startX = (ImGui::GetContentRegionAvail().x - totalButtonWidth) * 0.5f;
                ImGui::SetCursorPosX(startX);

                // Browser button
                if (!selectedArticle->url.empty()) {
                    if (ImGui::Button("Open in Browser", ImVec2(buttonWidth, 0))) {
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

                // Favorite button
                bool isFavorite = std::find_if(favorites.begin(), favorites.end(),
                                               [&](const NewsFetcher::NewsArticle& fav) {
                                                   return fav.title == selectedArticle->title;
                                               }) != favorites.end();

                pushIcon();
                if (isFavorite) {
                    ImGui::PushStyleColor(ImGuiCol_Button, colors.warning);
                    if (ImGui::Button((ICON_MINUS " Remove Favorite"), ImVec2(buttonWidth, 0))) {
                        removeFavorite(selectedArticle->title);
                    }
                    ImGui::PopStyleColor();
                } else {
                    if (ImGui::Button((ICON_PLUS " Add Favorite"), ImVec2(buttonWidth, 0))) {
                        addToFavorites(*selectedArticle);
                    }
                }
                popIcon();

                // Close button
                ImGui::SameLine();
                if (ImGui::Button("Close", ImVec2(buttonWidth, 0))) {
                    state.showArticlePopup = false;
                    if (state.showFavoritesPopup) {
                        state.showFavoritesPopup = true;
                    }
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }


    // Favorites popup implementation with safe removal and better error handling
    void NewsUI::renderFavoritesPopup() {
        ImGui::OpenPopup("Favorites");

        // Match size with article popup (85% of viewport)
        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        ImVec2 popupSize(viewportSize.x * 0.85f, viewportSize.y * 0.85f);
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(popupSize, ImGuiCond_Appearing);

        if (ImGui::BeginPopupModal("Favorites", &state.showFavoritesPopup)) {
            // Enhanced header with icon and count
            pushIcon();
            ImGui::PushStyleColor(ImGuiCol_Text, colors.warning);
            ImGui::Text(ICON_STAR);
            ImGui::PopStyleColor();
            popIcon();

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, colors.accent);
            ImGui::SetWindowFontScale(1.5f);
            ImGui::Text(" Saved Articles (%zu)", favorites.size());
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::Separator();
            ImGui::Spacing();

            // Content area with proper spacing
            const float footerHeight = ImGui::GetFrameHeightWithSpacing() * 2;
            ImGui::BeginChild("FavoritesList", ImVec2(0, -footerHeight), true);

            if (favorites.empty()) {
                ImVec2 contentSize = ImGui::GetContentRegionAvail();
                centerText("No favorites yet", contentSize.x, contentSize.y);
            } else {
                // Use index-based iteration to avoid iterator invalidation
                for (size_t i = 0; i < favorites.size(); i++) {
                    const auto& article = favorites[i];

                    ImGui::PushStyleColor(ImGuiCol_Text, colors.text);
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);

                    // Create unique ID for each article container
                    std::string childId = "Fav_" + std::to_string(i);
                    ImGui::BeginChild(childId.c_str(),
                                      ImVec2(ImGui::GetContentRegionAvail().x, 120), true);

                    // Title with proper text wrapping
                    float titleWidth = ImGui::GetContentRegionAvail().x - 40;
                    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + titleWidth);
                    ImGui::TextWrapped("%s", article.title.c_str());
                    ImGui::PopTextWrapPos();

                    if (!article.description.empty()) {
                        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s",
                                           article.description.substr(0, 150).c_str());
                    }

                    // Remove button with safe removal handling
                    ImGui::SetCursorPos(ImVec2(
                            ImGui::GetWindowWidth() - 40,
                            ImGui::GetCursorPosY() + 5
                    ));

                    pushIcon();
                    ImGui::PushStyleColor(ImGuiCol_Button, colors.error);
                    std::string buttonId = ICON_MINUS "##fav_" + std::to_string(i);

                    if (ImGui::Button(buttonId.c_str())) {
                        // Pop button color first
                        ImGui::PopStyleColor();
                        popIcon();

                        // Store title for removal
                        std::string titleToRemove = article.title;

                        // Pop remaining styles and close windows
                        ImGui::EndChild();
                        ImGui::PopStyleVar();
                        ImGui::PopStyleColor(); // Pop text color
                        ImGui::EndChild();
                        ImGui::EndPopup();

                        // Remove article after all windows are closed
                        removeFavorite(titleToRemove);
                        return;
                    }

                    ImGui::PopStyleColor();
                    popIcon();

                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Remove from favorites");
                    }

                    // Handle article click
                    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
                        selectedArticle = article;
                        state.showArticlePopup = true;
                        state.showFavoritesPopup = false;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                }
            }

            ImGui::EndChild();

            // Footer with close button
            ImGui::Separator();
            float buttonWidth = 120.0f;
            float centerPos = (ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f;
            ImGui::SetCursorPosX(centerPos);

            if (ImGui::Button("Close", ImVec2(buttonWidth, 0))) {
                state.showFavoritesPopup = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }


    // Renders the settings popup where the user can adjust the font size.
    void NewsUI::renderSettingsPopup() {
        if (state.showSettings) {
            ImGui::OpenPopup("Settings");
        }

        // Set popup size
        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        ImVec2 popupSize(viewportSize.x * 0.2f, viewportSize.y * 0.35f);
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(popupSize, ImGuiCond_Appearing);

        if (ImGui::BeginPopupModal("Settings", &state.showSettings)) {
            // Title with icon
            pushIcon();
            ImGui::Text(ICON_COG);
            popIcon();
            ImGui::SameLine();
            ImGui::Text(" Application Settings");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::BeginChild("SettingsContent", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2));

            // Theme settings
            ImGui::Text("Theme");
            bool themeChanged = ImGui::Checkbox("Dark Mode", &state.isDarkMode);
            if (themeChanged) {
                applyTheme();
                showMessage("Theme updated", colors.success);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();



            // Version info
            ImGui::Text("Version: %s\n\n Created by Hananel Sabag & Amit Cohen", APP_VERSION);

            ImGui::EndChild();

            // Close button
            if (ImGui::Button("Close", ImVec2(120, 0))) {
                state.showSettings = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }


    // Content management functions
    void NewsUI::handleSearch() {
        if (!state.searchQuery.empty()) {
            try {
                searchResults = fetcher.searchNews(state.searchQuery);
                state.showHome = false;
                if (!searchResults.empty()) {
                    showMessage("Search completed", colors.success);
                } else {
                    showMessage("No results found", colors.warning);
                }
            } catch (const std::exception& e) {
                showMessage("Search failed", colors.error);
                std::cerr << "[ERROR] Search failed: " << e.what() << std::endl;
            }
        }
    }

    void NewsUI::addToFavorites(const NewsFetcher::NewsArticle& article) {
        auto it = std::find_if(favorites.begin(), favorites.end(),
                               [&](const NewsFetcher::NewsArticle& fav) {
                                   return fav.title == article.title;
                               });

        if (it == favorites.end()) {
            favorites.push_back(article);
            NewsStorage::saveFavoritesToFile(favorites);
            showMessage("Added to favorites", colors.success);
        }
    }

    void NewsUI::removeFavorite(const std::string& title) {
        size_t initialSize = favorites.size();
        favorites.erase(
                std::remove_if(favorites.begin(), favorites.end(),
                               [&](const NewsFetcher::NewsArticle& article) {
                                   return article.title == title;
                               }),
                favorites.end()
        );

        if (favorites.size() < initialSize) {
            NewsStorage::saveFavoritesToFile(favorites);
            showMessage("Removed from favorites", colors.success);
        }
    }