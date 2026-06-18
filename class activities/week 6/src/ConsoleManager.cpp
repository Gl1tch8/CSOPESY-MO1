#include "ConsoleManager.h"
#include "Console.h"

ConsoleManager::ConsoleManager() {
    running = true;

    consoleTable[MAIN_CONSOLE] = std::make_shared<MainConsole>();

    currentConsole = consoleTable[MAIN_CONSOLE];
    currentConsole->setRunning(true);
}

ConsoleManager* ConsoleManager::singleton = nullptr;

void ConsoleManager::initialize() {
    singleton = new ConsoleManager();
}

ConsoleManager* ConsoleManager::getInstance() {
    return singleton;
}

void ConsoleManager::destroy() {
    delete singleton;
}

void ConsoleManager::start() {
    consoleThread = std::thread(&ConsoleManager::run, this);
}

void ConsoleManager::stop() {
    if (consoleThread.joinable()) {
        consoleThread.join();
    }
}

void ConsoleManager::run() {
    while (running) {
        currentConsole->run();
    }
}

bool ConsoleManager::isRunning() {
    return running;
}

bool ConsoleManager::registerScreen(std::string consoleName, bool isSwitch) {
    if (consoleTable.contains(consoleName)) {
        std::cerr << "Error: Screen " << consoleName << " already exists." << std::endl;
        return false;
    }

    consoleTable[consoleName] = std::make_shared<Screen>(consoleName);

    if (isSwitch) {
        switchConsole(consoleName);
    }

    return true;
}

void ConsoleManager::switchConsole(std::string consoleName) {
    if (!consoleTable.contains(consoleName)) {
        std::cerr << "Error: Screen " << consoleName << " not registered." << std::endl;
        return;
    }

    currentConsole->setRunning(false);
    currentConsole = consoleTable[consoleName];
    currentConsole->setRunning(true);
}

void ConsoleManager::terminate() {
    running = false;
}