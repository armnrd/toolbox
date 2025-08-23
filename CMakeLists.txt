cmake_minimum_required(VERSION 3.10)
project(toolbox VERSION 0.1 LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_PREFIX_PATH "C:/Packages/Qt/6.9.1/msvc2022_64")

find_package(Qt6 COMPONENTS REQUIRED
        Core
        Gui
        Widgets
)

# Fetch json v3.12.0
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

set(TOOLBOX_SOURCES
        resources.qrc
        main.cpp
        config.cpp
        config.hpp
        hotkeys.cpp
        hotkeys.hpp
        window_management.cpp
        window_management.hpp
        tray_icon.cpp
        tray_icon.hpp
        config_editor_frame.cpp
        config_editor_frame.hpp
        config_editor_frame.ui
        toolbox.hpp
        hacks.cpp
        hacks.hpp
        hacks_dpi_scaling.cpp
        hacks_dpi_scaling.hpp
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    qt_add_executable(toolbox WIN32 ${TOOLBOX_SOURCES})
else ()
    qt_add_executable(toolbox ${TOOLBOX_SOURCES})
endif ()

target_sources(toolbox PRIVATE "resources.rc")

target_link_libraries(toolbox PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        nlohmann_json::nlohmann_json
)

# Copy configuration, etc. files to the build directory after building the target
add_custom_command(
        TARGET toolbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/config.json $<TARGET_FILE_DIR:toolbox>
)

# Deployment for Windows to include all necessary DLLs and plugins
set(DEPLOYQT_FLAGS $<$<NOT:$<CONFIG:Debug>>:--release> --no-translations --no-compiler-runtime)
add_custom_command(TARGET toolbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running windeployqt..."
        COMMAND windeployqt.exe ${DEPLOYQT_FLAGS} $<TARGET_FILE:toolbox>
)
