cmake_minimum_required(VERSION 3.12)
set(ENV{VCPKG_FEATURE_FLAGS} "manifests,versions")
cmake_path(CONVERT $ENV{USERPROFILE} TO_CMAKE_PATH_LIST USERPROFILE_PATH)
set(CMAKE_TOOLCHAIN_FILE "${USERPROFILE_PATH}/.vcpkg-clion/scripts/buildsystems/vcpkg.cmake")
list(APPEND CMAKE_PREFIX_PATH "C:/Packages/Qt/6.9.2/msvc2022_64")

project(toolbox VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS REQUIRED
        Core
        Quick
        Gui
        Widgets
)

find_package(yaml-cpp CONFIG REQUIRED)

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
        tray.cpp
        tray.hpp
        keymap.cpp
        keymap.hpp
        windowmgr.cpp
        windowmgr.hpp
        workspaces.cpp
        workspaces.hpp
)

qt_standard_project_setup(REQUIRES 6.9)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    qt_add_executable(toolbox WIN32 ${TOOLBOX_SOURCES})
else ()
    qt_add_executable(toolbox ${TOOLBOX_SOURCES}) # TODO: find correct method for debugging
endif ()

#qt_add_qml_module(toolbox
#        URI toolbox
#        VERSION 1.0
#        QML_FILES
#        WorkspacesDashboard.qml
#)

target_link_libraries(toolbox PRIVATE
        Qt::Core
        Qt::Quick
        Qt::Gui
        Qt::Widgets
        yaml-cpp::yaml-cpp
)

# Target for copying configuration, etc. files to the build directory
add_custom_target(resource_files
        COMMAND ${CMAKE_COMMAND} -E echo_append "Copying resource files... "
        COMMAND ${CMAKE_COMMAND} -E copy ${TOOLBOX_RUNTIME_FILES} ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E echo "done."
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_dependencies(toolbox resource_files) # Make sure runtime files are copied while building

# Deployment for Windows to include all necessary DLLs and plugins
set(WINDEPLOYQT_FLAGS
        $<$<NOT:$<CONFIG:Debug>>:--release>
        --no-translations
        --no-compiler-runtime
)
add_custom_command(TARGET toolbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running windeployqt:"
        COMMAND ${WINDEPLOYQT_EXECUTABLE} ${WINDEPLOYQT_FLAGS} $<TARGET_FILE:toolbox>
)
