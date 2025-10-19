//
// Created by Edward on 8/23/2025.
//

#include "settings_window.h"

settings_window::settings_window()
    : window_base("Settings", false, ImGuiWindowFlags_None) {
    SetFlags(ImGuiWindowFlags_NoCollapse);
    ImGui::SetNextWindowSize(GetSize(), ImGuiCond_FirstUseEver);
}

void settings_window::draw_window() {
    if ((ImGui::GetWindowPos().x !=  GetPosition().x) ||  (ImGui::GetWindowPos().y != GetPosition().y)) {
        SetPosition(ImGui::GetWindowPos());
        SetSize(ImVec2(200, 400));
        ImGui::GetWindowPos();
        ImGui::SetWindowSize(GetSize(), ImGuiCond_FirstUseEver);
    }

    ImGui::Text("Application Settings");
    ImGui::Separator();
    ImGui::SliderFloat("Volume", &_volume, 0.0f, 1.0f, "%.2f");
    ImGui::Checkbox("Fullscreen", &_fullscreen);
    ImGui::Text("Theme:");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##theme", _themes[_selected_theme])) {
        for (int i = 0; i < 3; i++) {
            bool is_selected = (_selected_theme == i);
            if (ImGui::Selectable(_themes[i], is_selected)) {
                _selected_theme = i;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Separator();
    ImGui::Checkbox("Show Advanced", &_show_advanced);
    if (_show_advanced) {
        ImGui::Indent();
        ImGui::Text("Advanced Options");
        static bool vsync = true;
        static int msaa = 4;
        ImGui::Checkbox("VSync", &vsync);
        ImGui::SliderInt("MSAA", &msaa, 0, 16, "%dx");
        ImGui::Unindent();
    }
    ImGui::Separator();
    ImGui::Button("Apply");
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        Hide();
    }
}
