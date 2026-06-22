#pragma once

#include "math_utils.h"
#include <SDL3/SDL.h>
#include <string>

namespace sim {
    class Renderer {
    public:
        bool init(int width, int height, const std::string &title = "SDLSimulator");
        void render();

        SDL_Window *window() const { return window_; }

        /// Update the viewport after a window resize event.
        void resize(int width, int height);

        void shutdown() const;

    private:
        struct GLMesh {
            unsigned int VAO = 0;
            unsigned int VBO = 0;
            unsigned int EBO = 0;
            unsigned int indexCount = 0;
            math::Vec3 boundsMin;
            math::Vec3 boundsMax;
        };

        std::unordered_map<std::string, GLMesh> m_loadedMeshes;

        // SDL / GL state
        SDL_Window*   window_  = nullptr;
        SDL_GLContext glCtx_   = nullptr;
        int           width_   = 800;
        int           height_  = 600;

        unsigned int ID_ = 0;

        bool compileShaders();
    };
}
