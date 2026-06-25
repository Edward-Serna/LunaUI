## SDL3 ##
if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SDL/CMakeLists.txt")
    add_subdirectory(SDL EXCLUDE_FROM_ALL)
else()
    message(FATAL_ERROR "SDL submodule not found. Run: git submodule update --init --recursive")
endif()

## SDL3 Image ##
if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/SDL_image/include/SDL3_image/SDL_image.h")
    set(SDLIMAGE_VENDORED ON)
    set(SDLIMAGE_AVIF OFF)	# disable formats we don't use to make the build faster and smaller.
    set(SDLIMAGE_BMP OFF)
    set(SDLIMAGE_JPG OFF)
    set(SDLIMAGE_WEBP OFF)
    set(SDLIMAGE_PNG_LIBPNG OFF) # SDL3 includes built-in support for loading PNGs.
    add_subdirectory(SDL_image EXCLUDE_FROM_ALL)
else()
    message(FATAL_ERROR "SDL3 Image not found. Run: git submodule update --init --recursive")
endif()

## Assimp (with minimal features for OBJ loading) ##
if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/assimp/CMakeLists.txt")
    add_subdirectory(assimp EXCLUDE_FROM_ALL)
else()
    message(FATAL_ERROR "Assimp submodule not found. OBJ loading will be unavailable.
                    Run: git submodule update --init --recursive")
endif()

## GLAD (required for OpenGL loading) ##
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/glad/include/glad/glad.h")
    message(FATAL_ERROR "GLAD not found. Run: git submodule update --init --recursive")
endif()

## FMT (Console Output) ##
if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/fmt/CMakeLists.txt")
    add_subdirectory(fmt EXCLUDE_FROM_ALL)
else()
message(FATAL_ERROR "fmt submodule not found. Run: git submodule update --init --recursive")
endif()

