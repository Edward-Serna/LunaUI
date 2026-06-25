#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <optional>
#include "shaders/shader.h"

namespace sim {
    class Renderer {
    public:
        bool init(int width, int height, const std::string& title);
        SDL_Window *window() const { return window_; }
        void resize(int width, int height); // Update the viewport after a window resize event.
        void reload(Renderer &renderer) const;
        void end() const;
        void renderer() const;
        void shutdown() const;

        std::optional<Shader> ourShader;

    private:
        std::string title_ = "SDLSimulator";

        unsigned int VAO_ = 0;
        unsigned int VBO_ = 0;

        // SDL / GL state
        SDL_Window*   window_  = nullptr;
        SDL_GLContext glCtx_   = nullptr;
        int           width_   = 800;
        int           height_  = 600;
    };
}
