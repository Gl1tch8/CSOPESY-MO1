#include "Taskbar.hpp"

#include "imgui.h"

#include "TimeUtil.hpp"
#include "UIConfig.hpp"
#include "UIManager.hpp"

void Taskbar::draw() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float taskbarHeight = 60.0f * UIConfig::getScaleFactor();

    // Pin to the bottom of the screen, full width.
    ImGui::SetNextWindowPos(ImVec2(0, displaySize.y - taskbarHeight));
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, taskbarHeight));

    ImGui::Begin("Taskbar", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImVec2 btnSize = UIConfig::scale(ImVec2(120, 36));

    // Three required buttons. Two open custom screens, the third the Task Manager.
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

    drawSystemTray();

    ImGui::End();
}

void Taskbar::drawSystemTray() {
    // Right-aligned tray with dummy CPU/MEM readings and the live clock.
    float trayWidth = 320.0f * UIConfig::getScaleFactor();
    ImGui::SameLine(ImGui::GetWindowWidth() - trayWidth);

    // Dummy placeholder utilisation values.
    ImGui::Text("CPU: %.1f%%", 37.5f);
    ImGui::SameLine();
    ImGui::Text("MEM: %.1f%%", 52.0f);
    ImGui::SameLine();
    ImGui::Text("%s", getCurrentTimeString().c_str());
}
