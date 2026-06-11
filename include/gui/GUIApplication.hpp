#pragma once

#include <memory>

struct GLFWwindow;
class Desktop;
class Taskbar;

// Owns the GLFW window, OpenGL context, and Dear ImGui context, and runs the
// immediate-mode render loop (see imgui.pdf "The Main Application Loop").
class GUIApplication {
public:
    GUIApplication();
    ~GUIApplication();

    // Initialise GLFW/OpenGL/ImGui. Returns false on failure.
    bool init();

    // Run the render loop until the Desktop PWR button requests shutdown.
    void run();

private:
    void setupImGui();
    void updateLogic();
    void renderFrame();
    void shutdown();

    GLFWwindow* window = nullptr;
    bool initialized = false;

    std::unique_ptr<Desktop> desktop;
    std::unique_ptr<Taskbar> taskbar;
};
