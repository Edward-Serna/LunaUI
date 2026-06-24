#include "renderer.h"
#include "console.h"

#include <filesystem>
#include <glad/glad.h>

namespace sim {
    namespace {
        const std::filesystem::path RendererSourceDirectory = std::filesystem::path(__FILE__).parent_path();
        const std::filesystem::path ShaderDirectory = RendererSourceDirectory / "shaders";

        constexpr std::string_view DefaultFragmentShaderFile = "default.shader.frag";
        constexpr std::string_view DefaultVertexShaderFile = "default.shader.vert";

        std::string shaderPath( const std::string_view shaderFileName ) {
            return (ShaderDirectory / shaderFileName).string();
        }
    }

    bool Renderer::init( const int width, const int height, const std::string& title ) {
        width_ = width;
        height_ = height;

        // SDL3: SDL_Init returns true (non-zero) on success.
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            console::error("RENDERER", "SDL_Init failed: {}", SDL_GetError());
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
            console::error("RENDERER", "SDL_CreateWindow failed: {}", SDL_GetError());
            return false;
        }

        // Create OpenGL context
        glCtx_ = SDL_GL_CreateContext(window_);
        if (!glCtx_) {
            console::error("RENDERER", "SDL_GL_CreateContext failed: {}", SDL_GetError());
            return false;
        }

        SDL_GL_MakeCurrent(window_, glCtx_);

        // Set swap interval (vsync)
        SDL_GL_SetSwapInterval(0);

        // Initialize GLAD immediately after context creation
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            console::error("RENDERER", "Failed to initialize GLAD.");
            return false;
        }

        // Debug: Print OpenGL info
        console::debug("RENDERER", "OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        console::debug("RENDERER", "GLSL Version: {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
        console::debug("RENDERER", "Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        console::debug("RENDERER", "Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Set viewport
        glViewport(0, 0, width, height);

        // Set clear color (Background Color)
        glClearColor(0.12f, 0.14f, 0.28f, 1.0f);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        constexpr float vertices[] = {
                // positions        // colors
                0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom left
                0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top
            };

        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s),
        // and then configure vertex attributes(s).
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        const std::string vertexShaderPath = shaderPath(DefaultVertexShaderFile);
        const std::string fragmentShaderPath = shaderPath(DefaultFragmentShaderFile);

        console::debug("RENDERER", "Loading vertex shader: {}", vertexShaderPath);
        console::debug("RENDERER", "Loading fragment shader: {}", fragmentShaderPath);

        ourShader.emplace(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

        return true;
    }

    void Renderer::render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (ourShader) ourShader->use();
        glBindVertexArray(VAO_);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(window_);
    }

    void Renderer::resize( const int width, const int height ) {
        width_ = width;
        height_ = height;
        glViewport(0, 0, width, height);
    }

    void Renderer::shutdown() const {
        SDL_GL_DestroyContext(glCtx_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }
}