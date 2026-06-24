#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <optional>
#include "shaders/shader.h"

namespace sim {
    class Renderer {
    public:
        bool init(int width, int height, const std::string &title = "SDLSimulator");
        Renderer() = default;

        SDL_Window *window() const { return window_; }

        /// Update the viewport after a window resize event.
        void resize(int width, int height);
        void render();
        void shutdown() const;

        std::optional<Shader> ourShader;

    private:
        unsigned int VAO_ = 0;
        unsigned int VBO_ = 0;
        // unsigned int EBO_ = 0;

        // SDL / GL state
        SDL_Window*   window_  = nullptr;
        SDL_GLContext glCtx_   = nullptr;
        int           width_   = 800;
        int           height_  = 600;
    };
}
