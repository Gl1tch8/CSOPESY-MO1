#include "Desktop.hpp"

#include <string>

#include "imgui.h"

#include "TimeUtil.hpp"
#include "UIConfig.hpp"
#include "UIManager.hpp"

void Desktop::draw() {
    // Create a borderless full-screen window pinned behind everything else.
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::Begin("Desktop", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoBringToFrontOnFocus |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse);

    drawWallpaper();
    drawClock();
    drawPowerButton();
    drawLauncher();

    ImGui::End();
}

#include "TextureLoader.hpp"

void Desktop::loadResources() {
    // Multiple possible paths to find the wallpaper
    const char* paths[] = {
        "src/gui/guimg/GUIwallpaper.jpg",           // Running from project root
        "../../src/gui/guimg/GUIwallpaper.jpg",     // Running from out/build/
        "../../../src/gui/guimg/GUIwallpaper.jpg",  // Running from out/build/x64-Debug
        "GUIwallpaper.jpg"                          // If copied to the exe folder
    };

    for (const char* path : paths) {
        wallpaperTexture = TextureLoader::loadTexture(path, wallpaperWidth, wallpaperHeight);
        if (wallpaperTexture != 0) break;
    }
}

void Desktop::drawWallpaper() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 size = ImGui::GetIO().DisplaySize;

    if (wallpaperTexture != 0) {
        // Draw the loaded image texture
        drawList->AddImage((void*)(intptr_t)wallpaperTexture, ImVec2(0, 0), size);
    } else {
        // Fallback: Vertical gradient drawn directly via the window draw list.
        const ImU32 top = IM_COL32(28, 32, 64, 255);
        const ImU32 bottom = IM_COL32(36, 84, 96, 255);

        drawList->AddRectFilledMultiColor(
            ImVec2(0, 0), size,
            top, top, bottom, bottom);
    }
}

void Desktop::drawClock() {
    std::string timeStr = getCurrentTimeString();

    ImVec2 textSize = ImGui::CalcTextSize(timeStr.c_str());
    float margin = 20.0f * UIConfig::getScaleFactor();
    ImVec2 pos = ImVec2(ImGui::GetIO().DisplaySize.x - textSize.x - margin, margin);

    ImGui::SetCursorPos(pos);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", timeStr.c_str());
}

void Desktop::drawLauncher() {
    // Desktop "icons": large launcher buttons that open each application.
    // These complement the taskbar buttons with an obvious entry point.
    float margin = 20.0f * UIConfig::getScaleFactor();
    float top = 90.0f * UIConfig::getScaleFactor();
    ImVec2 btnSize = UIConfig::scale(ImVec2(140, 56));
    float spacing = 14.0f * UIConfig::getScaleFactor();

    struct App { const char* label; const char* window; };
    const App apps[] = {
        {"Task Manager", "Task Manager"},
        {"Settings",     "Settings"},
        {"About",        "About"},
    };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.28f, 0.42f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.40f, 0.60f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.34f, 0.50f, 0.74f, 1.0f));

    int i = 0;
    for (const App& app : apps) {
        ImGui::SetCursorPos(ImVec2(margin, top + i * (btnSize.y + spacing)));
        if (ImGui::Button(app.label, btnSize)) {
            UIManager::getInstance().showWindow(app.window);
        }
        ++i;
    }

    ImGui::PopStyleColor(3);
}

void Desktop::drawPowerButton() {
    // Shutdown button in the top-left corner. Clicking it is the only way to close the application.
    float margin = 20.0f * UIConfig::getScaleFactor();
    ImGui::SetCursorPos(ImVec2(margin, margin));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.12f, 0.12f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.90f, 0.20f, 0.20f, 1.0f));

    ImVec2 btnSize = UIConfig::scale(ImVec2(90, 40));
    if (ImGui::Button("PWR", btnSize)) {
        powerOff = true;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shut down");
    }

    ImGui::PopStyleColor(3);
}
