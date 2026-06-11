#pragma once

#include <string>

#include "imgui.h"

// Abstract base class for every closable window in the mockup
// (see imgui.pdf section 8). Subclasses implement draw(); beginWindow()/
// endWindow() keep ImGui::Begin()/End() balanced and honor visibility.
class AWindow {
public:
    explicit AWindow(const std::string& name)
        : windowName(name), isVisible(false) {}
    virtual ~AWindow() = default;

    // Subclasses must implement their own rendering.
    virtual void draw() = 0;

    // Optional per-frame logic; default no-op so UIManager can call it.
    virtual void update() {}

    void show() { isVisible = true; }
    void hide() { isVisible = false; }
    bool isShown() const { return isVisible; }

protected:
    // Begin an ImGui window with a built-in close (X) button bound to
    // isVisible. Returns false (and skips drawing) when hidden. On the first
    // frame after being shown, the window is centered and pulled to the front
    // so it never opens hidden behind the full-screen Desktop layer.
    bool beginWindow() {
        if (!isVisible) {
            wasVisible = false;
            return false;
        }
        if (!wasVisible) {
            ImVec2 display = ImGui::GetIO().DisplaySize;
            ImGui::SetNextWindowPos(ImVec2(display.x * 0.5f, display.y * 0.5f),
                                    ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(560, 380), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowFocus();
            wasVisible = true;
        }
        ImGui::Begin(windowName.c_str(), &isVisible);
        return true;
    }

    void endWindow() {
        ImGui::End();
    }

    std::string windowName;
    bool isVisible;
    bool wasVisible = false;
};
