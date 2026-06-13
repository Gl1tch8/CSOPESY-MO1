#pragma once

#include "AWindow.hpp"

// Settings screen opened from the taskbar. Demonstrates a custom
// layout with toggles and sliders (no real settings are applied).
class SettingsUI : public AWindow {
public:
    SettingsUI() : AWindow("Settings") {}

    void draw() override;

private:
    bool darkMode = true;
    bool notifications = false;
    bool wifi = true;
    float volume = 65.0f;
    float brightness = 80.0f;
};
