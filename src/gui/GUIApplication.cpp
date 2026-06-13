#include "GUIApplication.hpp"

#include <cstdio>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Silence macOS OpenGL deprecation warnings; harmless elsewhere.
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include "Desktop.hpp"
#include "Taskbar.hpp"
#include "UIConfig.hpp"
#include "UIManager.hpp"

namespace {
void glfwErrorCallback(int error, const char* description) {
    std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
} // namespace

GUIApplication::GUIApplication()
    : desktop(std::make_unique<Desktop>()),
      taskbar(std::make_unique<Taskbar>()) {}

GUIApplication::~GUIApplication() {
    shutdown();
}

bool GUIApplication::init() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to initialise GLFW\n");
        return false;
    }

    // Request an OpenGL 3.2 core context (GLSL #version 150).
    const char* glslVersion = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(1280, 720, "CSOPESY OS Mockup", nullptr, nullptr);
    if (!window) {
        std::fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync

    // Scale factor must be known before fonts/styles are set up.
    UIConfig::initialize();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    setupImGui();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // Load GUI assets like the wallpaper texture
    desktop->loadResources();

    initialized = true;
    return true;
}

void GUIApplication::setupImGui() {
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // don't litter an imgui.ini next to the binary

    ImGui::StyleColorsDark();

    // Use the built-in default font, scaled for DPI (no external TTF asset).
    io.Fonts->AddFontDefault();
    io.FontGlobalScale = UIConfig::getScaleFactor();
}

void GUIApplication::run() {
    if (!initialized) return;

    // Loop until the Desktop PWR button requests shutdown.A native window close (X / Cmd+Q) is intentionally ignored so the app can only be
    // closed via the PWR button.

    while (!desktop->shutdownRequested()) {
        glfwPollEvents();

        if (glfwWindowShouldClose(window)) {
            glfwSetWindowShouldClose(window, GLFW_FALSE);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        updateLogic();
        renderFrame();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

void GUIApplication::updateLogic() {
    UIManager::getInstance().updateAllWindows();
}

void GUIApplication::renderFrame() {
    desktop->draw();
    taskbar->draw();
    UIManager::getInstance().renderAllWindows();
}

void GUIApplication::shutdown() {
    if (!initialized) return;
    initialized = false;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
