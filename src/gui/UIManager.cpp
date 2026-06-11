#include "UIManager.hpp"

UIManager& UIManager::getInstance() {
    static UIManager instance;
    return instance;
}

void UIManager::registerWindow(const std::string& name, std::shared_ptr<AWindow> window) {
    windows[name] = window;
}

void UIManager::showWindow(const std::string& name) {
    auto it = windows.find(name);
    if (it != windows.end()) {
        it->second->show();
    }
}

void UIManager::hideWindow(const std::string& name) {
    auto it = windows.find(name);
    if (it != windows.end()) {
        it->second->hide();
    }
}

void UIManager::updateAllWindows() {
    for (auto& [name, window] : windows) {
        if (window->isShown()) {
            window->update();
        }
    }
}

void UIManager::renderAllWindows() {
    for (auto& [name, window] : windows) {
        if (window->isShown()) {
            window->draw();
        }
    }
}
