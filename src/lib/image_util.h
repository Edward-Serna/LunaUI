//
// Created by Edward on 11/30/2025.
//

#pragma once
#include <SDL3/SDL.h>

/**
 * Simple image utility for loading window icons
 * Uses STB image internally
 */
namespace ImageUtil {
    /**
     * Load an image as an SDL_Surface (useful for window icons)
     * @param filename Path to image file (PNG, JPG, BMP, etc.)
     * @return SDL_Surface* or nullptr on failure
     */
    SDL_Surface* LoadImage(const char* filename);

    /**
     * Set window icon from image file
     * @param window SDL_Window to set icon for
     * @param filename Path to icon image file
     * @return true on success, false on failure
     */
    bool SetWindowIcon(SDL_Window* window, const char* filename);
} // namespace ImageUtil