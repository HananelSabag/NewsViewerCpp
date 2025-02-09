cmake_minimum_required(VERSION 3.14)
project(NewsViewer)

set(CMAKE_CXX_STANDARD 17)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find OpenSSL (Required for HTTPS support in httplib)
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Define source files groups
set(CORE_SOURCES
        main.cpp
        src/news_fetcher.cpp
        src/news_storage.cpp
        src/ui.cpp
        )

set(IMGUI_SOURCES
        src/imgui.cpp
        src/imgui_draw.cpp
        src/imgui_tables.cpp
        src/imgui_widgets.cpp
        src/imgui_impl_glfw.cpp
        src/imgui_impl_opengl3.cpp
        )

# Combine all sources
add_executable(NewsViewer
        ${CORE_SOURCES}
        ${IMGUI_SOURCES}
        )

# Define preprocessor directive for OpenSSL support
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)

# Ensure asset files are copied to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Link required libraries
target_link_libraries(NewsViewer
        OpenGL::GL
        glfw3
        OpenSSL::SSL
        OpenSSL::Crypto
        ws2_32      # Windows Sockets
        crypt32     # Required for OpenSSL on Windows
        )

# Add compiler definitions for Unicode support
target_compile_definitions(NewsViewer PRIVATE
        UNICODE
        _UNICODE
        )