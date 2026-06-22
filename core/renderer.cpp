#include "renderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shaders/shader.h"

static const char* VERTEX_SHADER_SRC = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vNormal;
out vec3 vColor;
out vec3 vWorldPos;

void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    gl_Position   = uProjection * uView * worldPos;

    mat3 normalMat = transpose(inverse(mat3(uModel)));
    vNormal   = normalize(normalMat * aNormal);
    vColor    = aColor;
    vWorldPos = worldPos.xyz;
}
)GLSL";

static const char* FRAGMENT_SHADER_SRC = R"GLSL(
#version 330 core
in vec3 vNormal;
in vec3 vColor;

out vec4 FragColor;

void main() {
    vec3 lightDir = normalize(vec3(1.0, 2.0, 1.0));
    float diffuse = max(dot(vNormal, lightDir), 0.0);
    float ambient = 0.25;

    vec3 lit = vColor * (ambient + diffuse * 0.75);
    FragColor = vec4(lit, 1.0);
}
)GLSL";

namespace sim {
    bool Renderer::init( const int width, const int height, const std::string& title ) {
        width_ = width;
        height_ = height;

        // SDL3: SDL_Init returns true (non-zero) on success.
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
            return false;
        }

        // Set OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        // SDL3: Create window WITH SDL_WINDOW_OPENGL flag (required!)
        window_ = SDL_CreateWindow(title.c_str(),
                                    width, height,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
                                    // | SDL_WINDOW_MAXIMIZED
                                   );

        if (!window_) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
            return false;
        }

        // Create OpenGL context
        glCtx_ = SDL_GL_CreateContext(window_);
        if (!glCtx_) {
            std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_GL_MakeCurrent(window_, glCtx_);

        // Set swap interval (vsync)
        SDL_GL_SetSwapInterval(0);

        // Initialize GLAD immediately after context creation
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            std::cerr << "Failed to initialize GLAD\n";
            return false;
        }

        // Debug: Print OpenGL info
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
        std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
        std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
        std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Set viewport
        glViewport(0, 0, width, height);

        // Set clear color (Background Color)
        glClearColor(0.12f, 0.14f, 0.28f, 1.0f);



        return true;
    }

    void Renderer::shutdown() const {

        SDL_GL_DestroyContext(glCtx_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    bool Renderer::compileShaders() {

        return true;
    }


    void Renderer::render() {
        glViewport(0, 0, width_, height_);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(ID_);

        SDL_GL_SwapWindow(window_);
    }



    void Renderer::resize( const int width, const int height ) {
        width_ = width;
        height_ = height;
        glViewport(0, 0, width, height);
    }
}
