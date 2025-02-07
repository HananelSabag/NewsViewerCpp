cmake_minimum_required(VERSION 3.14)
project(NewsViewer)

set(CMAKE_CXX_STANDARD 17)

# מציאת OpenGL
find_package(OpenGL REQUIRED)

# מציאת OpenSSL
find_package(OpenSSL REQUIRED)

# הגדרת תיקיות include
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

# קבצי מקור (ודא שכולם מופיעים כאן)
add_executable(NewsViewer
        main.cpp
        src/news_fetcher.cpp
        src/ui.cpp
        src/imgui.cpp
        src/imgui_draw.cpp
        src/imgui_tables.cpp
        src/imgui_widgets.cpp
        src/imgui_impl_glfw.cpp
        src/imgui_impl_opengl3.cpp

        )
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)

# חיבור לספריות
target_link_libraries(NewsViewer
        OpenGL::GL  # OpenGL
        glfw3       # GLFW
        OpenSSL::SSL OpenSSL::Crypto # OpenSSL
        ws2_32      # Windows Sockets (נדרש עבור HTTP)
        crypt32
        )
