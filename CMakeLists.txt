cmake_minimum_required(VERSION 3.18)
project(toolbox VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(FetchContent)
#set(CMAKE_TOOLCHAIN_FILE "C:/Users/biswas02/.vcpkg-clion/scripts/buildsystems/vcpkg.cmake")

list(APPEND CMAKE_PREFIX_PATH "C:/Packages/Qt/6.9.2/msvc2022_64")

find_package(Qt6 COMPONENTS REQUIRED
        Core
        Quick
        Gui
        Widgets
)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG 0.8.0
)
FetchContent_MakeAvailable(yaml-cpp)

set(TOOLBOX_RUNTIME_FILES
        config.yaml
)

set(TOOLBOX_RESOURCE_FILES
        resources.rc
        resources.qrc
)

set(TOOLBOX_SOURCES
        ${TOOLBOX_RESOURCE_FILES}
        toolbox.cpp
        toolbox.hpp
        config.cpp
        config.hpp
        tray_icon.cpp
        tray_icon.hpp
        hotkeys.cpp
        hotkeys.hpp
        window_management.cpp
        window_management.hpp
        workspaces.cpp
        workspaces.hpp

        #        config_editor_frame.cpp
        #        config_editor_frame.hpp
        #        config_editor_frame.ui
        #        hacks_dpi_scaling.cpp
        #        hacks_dpi_scaling.hpp
        #        hacks.cpp
        #        hacks.hpp
)

qt_standard_project_setup(REQUIRES 6.9.2)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    qt_add_executable(toolbox WIN32 ${TOOLBOX_SOURCES})
else ()
    qt_add_executable(toolbox ${TOOLBOX_SOURCES}) # TODO: find correct method for debugging
endif ()

qt_add_qml_module(toolbox
        URI toolbox
        VERSION 1.0
        QML_FILES
        WorkspacesDashboard.qml
)

target_link_libraries(toolbox PRIVATE
        Qt::Core
        Qt::Quick
        Qt::Gui
        Qt::Widgets
        yaml-cpp::yaml-cpp
)

include(GNUInstallDirs)
install(TARGETS toolbox
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Target for copying configuration, etc. files to the build directory
add_custom_target(copy_runtime_files
        COMMAND ${CMAKE_COMMAND} -E copy ${TOOLBOX_RUNTIME_FILES} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Copying runtime files"
)

add_dependencies(toolbox copy_runtime_files) # Make sure runtime files are copied while building

# Deployment for Windows to include all necessary DLLs and plugins
set(DEPLOYQT_FLAGS
        $<$<NOT:$<CONFIG:Debug>>:--release>
        --no-translations
        --no-compiler-runtime
)
add_custom_command(TARGET toolbox POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE} ${DEPLOYQT_FLAGS} $<TARGET_FILE:toolbox>
        COMMENT "Running windeployqt..."
)
