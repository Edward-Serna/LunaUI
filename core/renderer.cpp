#include "renderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

static const char *VERTEX_SHADER_SRC = R"GLSL(
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

static const char *FRAGMENT_SHADER_SRC = R"GLSL(
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


// struct CubeVertex {
//     float pos[3];
//     float normal[3];
//     float color[3];
// };
//
// static const CubeVertex CUBE_VERTICES[24] = {
//     // Front (+Z)
//     {{0.5f, -0.5f, 0.5f}, {0, 0, 1}, {0.95f, 0.3f, 0.3f}},
//     {{0.5f, 0.5f, 0.5f}, {0, 0, 1}, {0.95f, 0.3f, 0.3f}},
//     {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {0.95f, 0.3f, 0.3f}},
//     {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {0.95f, 0.3f, 0.3f}},
//     // Back (-Z)
//     {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0.3f, 0.3f, 0.95f}},
//     {{-0.5f, 0.5f, -0.5f}, {0, 0, -1}, {0.3f, 0.3f, 0.95f}},
//     {{0.5f, 0.5f, -0.5f}, {0, 0, -1}, {0.3f, 0.3f, 0.95f}},
//     {{0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0.3f, 0.3f, 0.95f}},
//     // Top (+Y)
//     {{-0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0.3f, 0.95f, 0.3f}},
//     {{-0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0.3f, 0.95f, 0.3f}},
//     {{0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0.3f, 0.95f, 0.3f}},
//     {{0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0.3f, 0.95f, 0.3f}},
//     // Bottom (-Y)
//     {{-0.5f, -0.5f, 0.5f}, {0, -1, 0}, {0.95f, 0.85f, 0.2f}},
//     {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0.95f, 0.85f, 0.2f}},
//     {{0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0.95f, 0.85f, 0.2f}},
//     {{0.5f, -0.5f, 0.5f}, {0, -1, 0}, {0.95f, 0.85f, 0.2f}},
//     // Right (+X)
//     {{0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0.95f, 0.55f, 0.2f}},
//     {{0.5f, 0.5f, -0.5f}, {1, 0, 0}, {0.95f, 0.55f, 0.2f}},
//     {{0.5f, 0.5f, 0.5f}, {1, 0, 0}, {0.95f, 0.55f, 0.2f}},
//     {{0.5f, -0.5f, 0.5f}, {1, 0, 0}, {0.95f, 0.55f, 0.2f}},
//     // Left (-X)
//     {{-0.5f, -0.5f, 0.5f}, {-1, 0, 0}, {0.6f, 0.3f, 0.95f}},
//     {{-0.5f, 0.5f, 0.5f}, {-1, 0, 0}, {0.6f, 0.3f, 0.95f}},
//     {{-0.5f, 0.5f, -0.5f}, {-1, 0, 0}, {0.6f, 0.3f, 0.95f}},
//     {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0.6f, 0.3f, 0.95f}},
// };


static const unsigned short CUBE_INDICES[36] = {
    0, 1, 2, 0, 2, 3,
    4, 5, 6, 4, 6, 7,
    8, 9, 10, 8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
};


namespace sim {
    bool Renderer::init(int width, int height, const std::string &title) {
        m_width = width;
        m_height = height;

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
        m_window = SDL_CreateWindow(
            title.c_str(),
            width, height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
            // | SDL_WINDOW_MAXIMIZED
        );

        if (!m_window) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
            return false;
        }

        // Create OpenGL context
        m_glCtx = SDL_GL_CreateContext(m_window);
        if (!m_glCtx) {
            std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_GL_MakeCurrent(m_window, m_glCtx);

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

        // Compile shaders
        if (!compileShaders()) {
            std::cerr << "[Renderer] Failed to compile shaders\n";
            return false;
        }

        // Upload geometry
        // uploadCubeGeometry();

        return true;
    }

    void Renderer::shutdown() {
        glDeleteVertexArrays(1, &m_cubeVAO);
        glDeleteBuffers(1, &m_cubeVBO);
        glDeleteBuffers(1, &m_cubeEBO);
        glDeleteProgram(m_shaderProgram);

        SDL_GL_DestroyContext(m_glCtx);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    bool Renderer::compileShaders() {
        auto compile = [](const char *src, GLenum type) {
            GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &src, nullptr);
            glCompileShader(s);
            return s;
        };

        GLuint vs = compile(VERTEX_SHADER_SRC, GL_VERTEX_SHADER);
        GLuint fs = compile(FRAGMENT_SHADER_SRC, GL_FRAGMENT_SHADER);

        m_shaderProgram = glCreateProgram();
        glAttachShader(m_shaderProgram, vs);
        glAttachShader(m_shaderProgram, fs);
        glLinkProgram(m_shaderProgram);

        glDeleteShader(vs);
        glDeleteShader(fs);

        m_uModel = glGetUniformLocation(m_shaderProgram, "uModel");
        m_uView = glGetUniformLocation(m_shaderProgram, "uView");
        m_uProjection = glGetUniformLocation(m_shaderProgram, "uProjection");

        return true;
    }

    void Renderer::uploadCubeGeometry() {
        glGenVertexArrays(1, &m_cubeVAO);
        glBindVertexArray(m_cubeVAO);

        glGenBuffers(1, &m_cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
        // glBufferData(GL_ARRAY_BUFFER,sizeof(CUBE_VERTICES),CUBE_VERTICES,GL_STATIC_DRAW);

        glGenBuffers(1, &m_cubeEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(CUBE_INDICES),CUBE_INDICES,GL_STATIC_DRAW);

        // constexpr int stride = sizeof(CubeVertex);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *) 0);
        // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *) (12));
        // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void *) (24));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        m_cubeIndexCount = 36;
        glBindVertexArray(0);
    }

    void Renderer::render(const SceneNodePtr &scene,
                          const SceneNodePtr &camera) {
        glViewport(0, 0, m_width, m_height);
        glClearColor(0.12f, 0.14f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        math::Vec3 camPos = camera->transform.position;
        math::Vec3 target = camera->orbit
                                ? camera->orbit->target
                                : math::Vec3(0, 0, 0);

        math::Mat4 view = glm::lookAt(camPos, target, {0, 1, 0});

        math::Mat4 proj = glm::perspective(
            glm::radians(camera->camera->fovDeg),
            float(m_width) / float(m_height),
            camera->camera->nearPlane,
            camera->camera->farPlane
        );

        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(m_uView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(m_uProjection, 1, GL_FALSE, glm::value_ptr(proj));

        drawNode(scene, math::Mat4(1.0f));
        SDL_GL_SwapWindow(m_window);
    }

    void Renderer::drawNode(const SceneNodePtr &node, const math::Mat4 &parentWorld) {
        if (!node) return;

        math::Mat4 world = parentWorld * node->transform.toMatrix();

        if (node->type == NodeType::Mesh) {
            glUniformMatrix4fv(m_uModel, 1, GL_FALSE, glm::value_ptr(world));
            glBindVertexArray(m_cubeVAO);
            glDrawElements(GL_TRIANGLES, m_cubeIndexCount, GL_UNSIGNED_SHORT, nullptr);
        }

        for (auto &child: node->children)
            drawNode(child, world);
    }

    void Renderer::resize(int w, int h) {
        m_width = w;
        m_height = h;
        glViewport(0, 0, w, h);
    }
}
