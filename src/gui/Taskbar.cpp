#include "Taskbar.hpp"

#include "imgui.h"

#include "UIConfig.hpp"
#include "UIManager.hpp"

void Taskbar::draw() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float taskbarHeight = 60.0f * UIConfig::getScaleFactor();

    // Pin to the bottom of the screen, full width.
    ImGui::SetNextWindowPos(ImVec2(0, displaySize.y - taskbarHeight));
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, taskbarHeight));

    // NOTE: no NoBringToFrontOnFocus here. That flag belongs only to the
    // full-screen Desktop background window; if the Taskbar also had it, it
    // would share the "always at back" layer and get hidden behind the Desktop.
    ImGui::Begin("Taskbar", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoScrollbar);

    ImVec2 btnSize = UIConfig::scale(ImVec2(120, 36));

    // Vertically center the row of buttons within the taskbar height.
    float verticalPad = (taskbarHeight - btnSize.y) * 0.5f;
    if (verticalPad > 0.0f) {
        ImGui::SetCursorPosY(verticalPad);
    }

    // Three required buttons laid out horizontally (each on the same line).
    // Two open custom screens, the third the Task Manager.
    if (ImGui::Button("Settings", btnSize)) {
        UIManager::getInstance().showWindow("Settings");
    }
    ImGui::SameLine();
    if (ImGui::Button("About", btnSize)) {
        UIManager::getInstance().showWindow("About");
    }
    ImGui::SameLine();
    if (ImGui::Button("Task Manager", btnSize)) {
        UIManager::getInstance().showWindow("Task Manager");
    }

    ImGui::End();
}
