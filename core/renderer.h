#pragma once

#include "scene_node.h"
#include "math_utils.h"
#include "asset_loader.h"
#include <SDL3/SDL.h>
#include <string>

namespace sim {
    class Renderer {
    public:
        bool init(int width, int height, const std::string &title = "SDLSimulator");

        void shutdown();

        void render(const SceneNodePtr &scene, const SceneNodePtr &camera);

        SDL_Window *window() const { return m_window; }

        /// Update the viewport after a window resize event.
        void resize(int width, int height);

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
        SDL_Window*   m_window  = nullptr;
        SDL_GLContext m_glCtx   = nullptr;
        int           m_width   = 800;
        int           m_height  = 600;

        // Shader program
        unsigned int m_shaderProgram = 0;
        int m_uModel      = -1;
        int m_uView       = -1;
        int m_uProjection = -1;

        // Cube geometry
        unsigned int m_cubeVAO = 0;
        unsigned int m_cubeVBO = 0;
        unsigned int m_cubeEBO = 0;
        int          m_cubeIndexCount = 0;

        bool compileShaders();
        void uploadCubeGeometry();
        void uploadMeshGeometry(const std::shared_ptr<Mesh_Data>& meshData,
                               const std::string& key);
        void drawNode(const SceneNodePtr &node, const math::Mat4 &parentWorld);
    };
}
