#include "UIConfig.hpp"

#include <GLFW/glfw3.h>

float UIConfig::scaleFactor = 1.0f;

void UIConfig::initialize() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        scaleFactor = 1.0f;
        return;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) {
        scaleFactor = 1.0f;
        return;
    }

    // Base design resolution is 1920px wide.
    const float baseWidth = 1920.0f;
    const float currentWidth = static_cast<float>(mode->width);
    scaleFactor = currentWidth / baseWidth;

    // Clamp to a reasonable range so the UI never gets unusably small/large.
    if (scaleFactor < 1.0f) scaleFactor = 1.0f;
    if (scaleFactor > 2.0f) scaleFactor = 2.0f;
}

float UIConfig::getScaleFactor() {
    return scaleFactor;
}

ImVec2 UIConfig::scale(ImVec2 size) {
    return ImVec2(size.x * scaleFactor, size.y * scaleFactor);
}
