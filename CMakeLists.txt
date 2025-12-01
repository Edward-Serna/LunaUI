cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(LunaUI)

set(CMAKE_FIND_LIBRARY_PREFER_SHARED TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

#################################################################
# SDL is configured for our CMake project
#################################################################
set(SDL_STATIC ON)
set(SDL_SHARED ON)
add_subdirectory(thirdparty/sdl)


find_package(OpenGL REQUIRED)# Ensures OpenGL is added

#################################################################
# Image Utility Library using stb_image header
#################################################################
add_library(image_util STATIC
        src/lib/image_util.cpp
        src/lib/image_util.h
)
target_include_directories(image_util
        PUBLIC ${PROJECT_SOURCE_DIR}/src/lib
        PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/sdl/include
)
target_link_libraries(image_util PUBLIC SDL3::SDL3-static)

add_executable(LunaUI
        src/main.cpp
        "${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui.cpp"
        "${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp"
        "${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp"
        "${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_tables.cpp"
        "${PROJECT_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp"
        "${PROJECT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_sdl3.cpp"
        "${PROJECT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_opengl3.cpp"
        src/app.cpp
        src/app.h
        src/draw/imgui_windows/window_manager.h
        src/draw/imgui_windows/window_base.cpp
        src/draw/imgui_windows/window_base.h
        src/draw/imgui_windows/settings_window.cpp
        src/draw/imgui_windows/settings_window.h
)

target_link_libraries(LunaUI PUBLIC
        SDL3::SDL3-static
        image_util # Self created lib
        ${OPENGL_LIBRARIES}
)

target_include_directories(LunaUI PRIVATE
        ${PROJECT_BINARY_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/thirdparty/imgui
        ${PROJECT_SOURCE_DIR}/thirdparty/imgui/backends
        ${PROJECT_SOURCE_DIR}/thirdparty/sdl/include
        ${PROJECT_SOURCE_DIR}/resources/images
        ${PROJECT_SOURCE_DIR}/src/lib
)

configure_file(
        ${PROJECT_SOURCE_DIR}/thirdparty/imgui/misc/fonts/Roboto-Medium.ttf
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Roboto-Medium.ttf
        COPYONLY
)