#include "SettingsUI.hpp"

#include "imgui.h"

void SettingsUI::draw() {
    if (!this->beginWindow()) return;

    ImGui::TextDisabled("System Settings (placeholder)");
    ImGui::Separator();

    ImGui::SeparatorText("Appearance");
    ImGui::Checkbox("Dark mode", &darkMode);
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 100.0f, "%.0f%%");

    ImGui::SeparatorText("Sound");
    ImGui::SliderFloat("Volume", &volume, 0.0f, 100.0f, "%.0f%%");

    ImGui::SeparatorText("Network & Notifications");
    ImGui::Checkbox("Wi-Fi", &wifi);
    ImGui::Checkbox("Notifications", &notifications);

    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Apply")) {
        // Placeholder — nothing is persisted.
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
        hide();
    }

    this->endWindow();
}
