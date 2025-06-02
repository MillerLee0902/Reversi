cmake_minimum_required(VERSION 3.29)
project(midterm_new LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 設置靜態連結選項
set(BUILD_SHARED_LIBS OFF)

# 針對不同編譯器的靜態連結設定
if (MSVC)
    # MSVC 編譯器的靜態連結設定
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options("/MT$<$<CONFIG:Debug>:d>")
else ()
    # MinGW/GCC 編譯器的靜態連結設定
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif ()

include(FetchContent)
# 設置SFML為靜態庫
set(SFML_BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(SFML_STATIC_LIBRARIES TRUE CACHE BOOL "Enable static libraries" FORCE)

FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

file(GLOB SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/headers/*.h")
if (SRC_FILES)
    add_executable(${PROJECT_NAME} ${SRC_FILES}
            src/AIDifficultySelection.cpp
            headers/AIDifficultySelection.h
            src/NetworkManager.cpp
            headers/NetworkManager.h
            src/NetworkMenu.cpp
            headers/NetworkMenu.h
            src/SimpleNetworkMenu.cpp
            headers/SimpleNetworkMenu.h
            src/NetworkGameScreen.cpp
            headers/NetworkGameScreen.h
            src/UpdatedNetworkMenu.cpp
            headers/UpdatedNetworkMenu.h
            src/NetworkGameClient.cpp
            headers/NetworkGameClient.h
            src/ImprovedNetworkMenu.cpp
            headers/ImprovedNetworkMenu.h
            headers/test_config.h
            src/ScrollableTextBox.cpp
            headers/ScrollableTextBox.h
    )
endif ()


target_link_libraries(${PROJECT_NAME} PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
        SFML::Network    # 添加這行 - 這是關鍵！
)

# Windows 特定的網路庫連結
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 wsock32)
endif()

file(COPY assets/textures DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY assets/fonts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY assets/sounds DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY assets/music DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
