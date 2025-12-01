//
// Created by Edward on 11/30/2025.
//

#include "image_util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

namespace ImageUtil {
    SDL_Surface* LoadImage(const char* filename) {
        int width, height, channels;
        // Load image with STB - automatically detects a format
        unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

        if (!data) {
            SDL_Log("Failed to load image '%s': %s", filename, stbi_failure_reason());
            return nullptr;
        }

        // Create SDL surface from loaded data
        SDL_Surface* surface = SDL_CreateSurfaceFrom(
            width,
            height,
            SDL_PIXELFORMAT_RGBA32,
            data,
            width * 4
        );

        if (!surface) {
            SDL_Log("Failed to create surface: %s", SDL_GetError());
            stbi_image_free(data);
            return nullptr;
        }
        return surface; // Note: surface now owns the data, will be freed when surface is destroyed
    }

    bool SetWindowIcon(SDL_Window* window, const char* filename) {
        SDL_Surface* icon = LoadImage(filename);
        if (!icon) {
            return false;
        }
        SDL_SetWindowIcon(window, icon);
        SDL_DestroySurface(icon);
        return true;
    }

} // namespace ImageUtil