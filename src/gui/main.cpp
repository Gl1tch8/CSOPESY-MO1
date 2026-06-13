#include <memory>

#include "GUIApplication.hpp"
#include "UIManager.hpp"

#include "AboutUI.hpp"
#include "SettingsUI.hpp"
#include "TaskManagerUI.hpp"

// Entry point for the GUI part, the console scheduler has its own entry point in src/main.cpp; this is a separate executable.
int main() {
    GUIApplication app;
    if (!app.init()) {
        return 1;
    }

    // Register the taskbar-launched windows with the global registry. The
    // names match the strings the Taskbar uses in showWindow(...).
    UIManager& ui = UIManager::getInstance();
    ui.registerWindow("Task Manager", std::make_shared<TaskManagerUI>());
    ui.registerWindow("Settings", std::make_shared<SettingsUI>());
    ui.registerWindow("About", std::make_shared<AboutUI>());

    app.run();
    return 0;
}
