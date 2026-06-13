#pragma once

#include <map>
#include <memory>
#include <string>

#include "AWindow.hpp"

// Singleton registry of all windows. Mirrors how a
// real OS compositor (DWM / WindowServer / Wayland) tracks window lifecycle.
class UIManager {
public:
    static UIManager& getInstance();

    void registerWindow(const std::string& name, std::shared_ptr<AWindow> window);
    void showWindow(const std::string& name);
    void hideWindow(const std::string& name);

    void updateAllWindows();
    void renderAllWindows();

private:
    UIManager() = default;
    std::map<std::string, std::shared_ptr<AWindow>> windows;
};
