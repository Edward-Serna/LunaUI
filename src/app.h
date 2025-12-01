//
// Created by Edward on 8/18/2025.
//

#pragma once

#include <memory>
#include <SDL3/SDL.h>

#include "draw/imgui_windows/window_manager.h"
#include "draw/imgui_windows/settings_window.h"

class app {
public:
    app();
    ~app();

    bool start_renderer();
    void render_loop() const;

protected:
    static void SetupImGuiStyle();

private:
    SDL_GLContext _gl_context = nullptr;
    SDL_Window* _window = nullptr;

    std::unique_ptr<window_manager> _imgui_window_manager;

    void InitializeWindows() const;
};
