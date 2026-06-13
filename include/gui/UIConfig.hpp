#pragma once

#include "imgui.h"

// DPI / resolution scaling helper.
// Computes a global scale factor from the primary monitor width so that UI
// elements stay a consistent physical size across different resolutions.
class UIConfig {
public:
    // Query the primary monitor and derive the scale factor (clamped 1.0-2.0).
    static void initialize();

    static float getScaleFactor();

    // Scale a size vector by the current factor.
    static ImVec2 scale(ImVec2 size);

private:
    static float scaleFactor;
};
