#pragma once

#include "AWindow.hpp"

// Placeholder About / System Info screen opened from the taskbar.
class AboutUI : public AWindow {
public:
    AboutUI() : AWindow("About") {}

    void draw() override;
};
