#pragma once

#include "AWindow.hpp"

// About / System Info screen opened from the taskbar.
class AboutUI : public AWindow {
public:
    AboutUI() : AWindow("About") {}

    void draw() override;
};
