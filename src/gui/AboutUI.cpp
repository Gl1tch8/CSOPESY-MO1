#include "AboutUI.hpp"

#include "imgui.h"

#include "UIConfig.hpp"

void AboutUI::draw() {
    if (!this->beginWindow()) return;

    ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "CSOPESY OS Mockup");
    ImGui::Separator();

    ImGui::Text("A Dear ImGui desktop-shell mockup built on the");
    ImGui::Text("GLFW + OpenGL + Dear ImGui immediate-mode stack.");
    ImGui::Spacing();

    ImGui::SeparatorText("System Information (placeholder)");
    ImGui::BulletText("Edition:        CSOPESY Shell 1.0");
    ImGui::BulletText("Renderer:       OpenGL 3 / Dear ImGui");
    ImGui::BulletText("Window manager: UIManager (singleton registry)");
    ImGui::BulletText("DPI scale:      %.2fx", UIConfig::getScaleFactor());
    ImGui::BulletText("Build:          %s %s", __DATE__, __TIME__);

    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Close")) {
        hide();
    }

    this->endWindow();
}
