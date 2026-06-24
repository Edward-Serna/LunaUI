#include "renderer.h"
#include "scene_node.h"
#include "scene_serializer.h"
#include "console.h"

#include <chrono>

using namespace sim;

static Scene buildDefaultScene() {
    // Root (empty grouping node)
    const auto root = makeEmpty("Root");

    Scene scene;
    scene.name    = "default";
    scene.root    = root;
    return scene;
}

int main() {
    using Clock = std::chrono::high_resolution_clock;

    buildDefaultScene();

    console::info("MAIN", "Initializing renderer...");

    Renderer renderer;
    if (!renderer.init(900, 700, "SDLSimulator")) {
        console::error("MAIN", "Failed to initialize renderer.");
        return 1;
    }

    console::success("MAIN", "Renderer initialized successfully.");

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
                if (evt.button.button == SDL_BUTTON_LEFT) mouseDragging = true;
                // Pause auto-orbit while the user is dragging
                // if (camera->orbit) camera->orbit->autoOrbit = false;
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (evt.button.button == SDL_BUTTON_LEFT) mouseDragging = false;
                break;

            case SDL_EVENT_WINDOW_RESIZED: {
                int w, h;
                SDL_GetWindowSize(renderer.window(), &w, &h);
                renderer.resize(w, h);
                break;
            }
            default:
                break;
            }
        }

        renderer.render();
    }

    renderer.shutdown();
    return 0;
}