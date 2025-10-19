cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(summer2025)

set(CMAKE_FIND_LIBRARY_PREFER_SHARED TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set(SDL_STATIC ON)
set(SDL_SHARED OFF)

add_subdirectory(thirdparty/sdl)

#set(SDL3IMAGE_INSTALL OFF)
#set(SDL3IMAGE_DEPS_SHARED OFF)
#set(SDL3IMAGE_VENDORED ON)
#set(SDL3IMAGE_BUILD_SHARED_LIBS OFF)

#add_subdirectory(thirdparty/sdl_image)

find_package(OpenGL REQUIRED)
#find_package(SDL3_image REQUIRED)

add_executable(summer2025
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

target_link_libraries(summer2025 PRIVATE
        SDL3::SDL3-static
#        SDL3_image::SDL3_image-static
        ${OPENGL_LIBRARIES}
)

target_include_directories(summer2025 PRIVATE
        ${PROJECT_BINARY_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/thirdparty/imgui
        ${PROJECT_SOURCE_DIR}/thirdparty/imgui/backends
        ${PROJECT_SOURCE_DIR}/thirdparty/sdl/include
#        ${PROJECT_SOURCE_DIR}/thirdparty/sdl_image/include
        ${PROJECT_SOURCE_DIR}/resources/images
        ${PROJECT_SOURCE_DIR}/src/lib
)

configure_file(
        ${PROJECT_SOURCE_DIR}/thirdparty/imgui/misc/fonts/Roboto-Medium.ttf
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Roboto-Medium.ttf
        COPYONLY
)