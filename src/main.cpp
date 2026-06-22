#include "renderer.h"
#include "scene_node.h"
#include "scene_serializer.h"

#include <fmt/color.h>
#include <fmt/chrono.h>

#include <iostream>

using namespace sim;

static auto prefixTime() {
    auto now = std::chrono::utc_clock::now();
    fmt::print("{:%Y-%m-%d %H:%M:%S} ", now);
}
static auto prefixMain() {
    fmt::print(fmt::fg(fmt::color::white) |
                  fmt::bg(fmt::color::dark_blue)," [MAIN] ");
}

static Scene buildDefaultScene() {

    // Root (empty grouping node)
    const auto root = makeEmpty("Root");

    // A unit cube at the origin, scaled up to 1.5×
    // auto cube = makeMesh("Cube", MeshType::Cube);
    // cube->transform.scale = { 1.5f, 1.5f, 1.5f };
    // root->children.push_back(cube);

    // Orbit camera looking at the origin
    // OrbitData orbit {
    //     .radius    = 5.0f,
    //     .azimuth   = 0.0f,
    //     .elevation = 0.9f,       // ~23° above horizontal
    //     .target    = { 0.0f, 0.0f, 0.0f },
    //     .speed     = 0.5f,       // radians/sec
    //     .autoOrbit = true
    // };
    CameraData cam ;

    // auto camera = makeOrbitCamera("MainCamera", orbit, cam);
    // root->children.push_back(camera);

    Scene scene;
    scene.name    = "default";
    scene.root    = root;
    return scene;
}

int main() {
    using namespace sim;
    using Clock = std::chrono::high_resolution_clock;

    buildDefaultScene();
    Renderer renderer;
    prefixTime();
    prefixMain();
    fmt::print("{}\n", fmt::styled(" Initializing renderer...",fmt::fg(fmt::color::white_smoke)
                    | fmt::bg(fmt::color::dodger_blue) | fmt::emphasis::bold));

    if (!renderer.init(900, 700, "SDLSimulator")) {
        std::cerr << "[main] Failed to initialize renderer.\n";
        return 1;
    }

    prefixTime();
    prefixMain();
    fmt::print("{}\n", fmt::styled(" Renderer initialized successfully. ",fmt::fg(fmt::color::white_smoke)
                    | fmt::bg(fmt::color::dodger_blue) | fmt::emphasis::bold));

    // Event loop
    bool     running       = true;
    bool     mouseDragging = false;
    auto     lastTime      = Clock::now();

    while (running) {
        // Delta time
        auto  now = Clock::now();
        float dt  = std::chrono::duration<float>(now - lastTime).count();
        lastTime  = now;
        // Clamp dt to avoid huge jumps if the window is dragged/resized
        if (dt > 0.2f) dt = 0.2f;

        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            switch (evt.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    // SDL3: evt.key.key replaces evt.key.keysym.sym
                    if (evt.key.key == SDLK_ESCAPE) running = false;
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (evt.button.button == SDL_BUTTON_LEFT) {
                        mouseDragging = true;
                        // Pause auto-orbit while the user is dragging
                        // if (camera->orbit) camera->orbit->autoOrbit = false;
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (evt.button.button == SDL_BUTTON_LEFT) {
                        mouseDragging = false;
                    }
                    break;
                // SDL3: Get window size directly (event fields vary by SDL3 version)
                case SDL_EVENT_WINDOW_RESIZED: {
                    int w, h;
                    SDL_GetWindowSize(renderer.window(), &w, &h);
                    renderer.resize(w, h);
                    break;
                }
                default: ;
            }
        }

        renderer.render();
    }
    renderer.shutdown();
    return 0;
}
