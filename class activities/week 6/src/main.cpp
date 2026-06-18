#include "globals.h"
#include "ConsoleManager.h"
#include "Scheduler.h"

int main() {
    ConsoleManager::initialize();
    Scheduler::initialize();
    bool running = true;

    ConsoleManager::getInstance()->start();

    while (running) {
        running = ConsoleManager::getInstance()->isRunning();
    }

    ConsoleManager::getInstance()->stop();
    Scheduler::getInstance()->stop();
    ConsoleManager::destroy();
    Scheduler::destroy();

    std::cout << "Program stopped successfully.\n";

    return 0;
};