#include "renderer.h"
#include "scene_node.h"
#include "scene_serializer.h"
#include "orbit_camera.h"
#include "asset_loader.h"

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
    auto root = makeEmpty("Root");

    // A unit cube at the origin, scaled up to 1.5×
    // auto cube = makeMesh("Cube", MeshType::Cube);
    // cube->transform.scale = { 1.5f, 1.5f, 1.5f };
    // root->children.push_back(cube);

    // Orbit camera looking at the origin
    OrbitData orbit {
        .radius    = 5.0f,
        .azimuth   = 0.0f,
        .elevation = 0.9f,       // ~23° above horizontal
        .target    = { 0.0f, 0.0f, 0.0f },
        .speed     = 0.5f,       // radians/sec
        .autoOrbit = true
    };
    CameraData cam { .fovDeg = 60.0f, .nearPlane = 0.1f, .farPlane = 1000.0f };

    auto camera = makeOrbitCamera("MainCamera", orbit, cam);
    root->children.push_back(camera);

    Scene scene;
    scene.name    = "default";
    scene.root    = root;
    scene.cameras = { camera };
    return scene;
}

int main() {
    using namespace sim;
    using Clock = std::chrono::high_resolution_clock;

    // ── Load or create scene ────────────────────────────
    Scene scene = SceneSerializer::load("default.json", "scenes");
    if (!scene.root) {
        prefixTime();
        prefixMain();
        fmt::print("{}\n", fmt::styled(" No saved scene found — building default.",fmt::fg(fmt::color::red)));

        scene = buildDefaultScene();
        SceneSerializer::save(scene, "scenes");   // persist immediately
    }

    // Grab the first camera
    SceneNodePtr camera = scene.cameras.empty() ? nullptr : scene.cameras[0];
    if (!camera) {
        prefixTime();
        prefixMain();
        fmt::print("{}\n", fmt::styled(" Scene has no camera — cannot continue.",fmt::fg(fmt::color::red)));
        return 1;
    }

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

    // ── Event loop ───────── ─────────────────────────────
    bool     running       = true;
    bool     mouseDragging = false;
    auto     lastTime      = Clock::now();

    // In main function, after scene loading:
    // Test asset loading
    auto& assetLoader = sim::AssetLoader::getInstance();

    auto testMesh = assetLoader.loadMesh("resource/assets/suzanne.obj");
    if (testMesh) {
        std::cout << "[main] Successfully loaded mesh with "
                  << testMesh->vertices.size() << " vertices\n";

        // Create a node for the loaded mesh
        auto suzanneNode = makeEmpty("Suzanne");
        suzanneNode->transform.position = {2.0f, 0.0f, 0.0f};
        suzanneNode->transform.scale = {0.5f, 0.5f, 0.5f};
        // You would need to extend the SceneNode to store actual mesh data
        scene.root->children.push_back(suzanneNode);
    }

    while (running) {
        // ── Delta time ──────────────────────────────────
        auto  now = Clock::now();
        float dt  = std::chrono::duration<float>(now - lastTime).count();
        lastTime  = now;
        // Clamp dt to avoid huge jumps if the window is dragged/resized
        if (dt > 0.2f) dt = 0.2f;

        // ── SDL Events ──────────────────────────────────
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            switch (evt.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    // SDL3: evt.key.key replaces evt.key.keysym.sym
                    if (evt.key.key == SDLK_ESCAPE) running = false;
                    // Toggle auto-orbit with spacebar
                    if (evt.key.key == SDLK_SPACE && camera->orbit) {
                        camera->orbit->autoOrbit = !camera->orbit->autoOrbit;
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (evt.button.button == SDL_BUTTON_LEFT) {
                        mouseDragging = true;
                        // Pause auto-orbit while the user is dragging
                        if (camera->orbit) camera->orbit->autoOrbit = false;
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (evt.button.button == SDL_BUTTON_LEFT) {
                        mouseDragging = false;
                    }
                    break;

                case SDL_EVENT_MOUSE_MOTION:
                    // SDL3: xrel / yrel are float
                    if (mouseDragging && camera->orbit) {
                        OrbitCamera::applyMouseDeltaX(*camera->orbit, evt.motion.xrel);
                        OrbitCamera::applyMouseDeltaY(*camera->orbit, evt.motion.yrel);
                    }
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    if (camera->orbit) {
                        // SDL3: wheel.y is float
                        OrbitCamera::applyZoom(*camera->orbit, evt.wheel.y);
                    }
                    break;

                // SDL3: Get window size directly (event fields vary by SDL3 version)
                case SDL_EVENT_WINDOW_RESIZED: {
                    int w, h;
                    SDL_GetWindowSize(renderer.window(), &w, &h);
                    renderer.resize(w, h);
                    break;
                }
            }
        }

        OrbitCamera::update(camera, dt);

        renderer.render(scene.root, camera);
    }

    std::cerr << "[main] Saving scene before exit…\n";
    SceneSerializer::save(scene, "scenes");

    renderer.shutdown();
    return 0;
}
