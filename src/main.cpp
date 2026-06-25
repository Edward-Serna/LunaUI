#include "renderer.h"
#include "utility/console.h"

using namespace sim;

int main() {
    console::info("MAIN", "Initializing renderer...");

    Renderer renderer;
    if (!renderer.init(900, 700, "SDLSimulator"))
        console::error("MAIN", "Failed to initialize renderer.");
    console::success("MAIN", "Renderer initialized successfully.");

    //////////////// Event Loop ////////////////
    bool     running       = true;
    // bool     mouseDragging = false;
    // auto     lastTime      = Clock::now();
    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();

    while (running) {
        frameCount++;
        if (Uint32 currentTime = SDL_GetTicks(); currentTime - startTime >= 1000) {
            unsigned int fps = frameCount / ((currentTime - startTime) / 1000.0f);
            console::debug("System", "FPS: {}", fps);
            frameCount = 0; // Reset counters
            startTime = currentTime;
        }

        renderer.renderer();
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
                if (evt.button.button == SDL_BUTTON_LEFT) renderer.reload(renderer);
                // Pause auto-orbit while the user is dragging
                // if (camera->orbit) camera->orbit->autoOrbit = false;
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                // if (evt.button.button == SDL_BUTTON_LEFT) mouseDragging = false;
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
    }
    renderer.shutdown();
    return 0;
}