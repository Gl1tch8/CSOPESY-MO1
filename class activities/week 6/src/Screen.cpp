#include "Screen.h"
#include "ConsoleManager.h"

Screen::Screen(std::string name) {
    this->name = name;
}

void Screen::run() {
    std::string cmd;
    
    std::cout << "\033[2J\033[H";
    std::cout << "Screen name: " << name << std::endl;

    while(running) {
        std::cout << std::endl;
        std::cout << "> ";
        std::getline(std::cin, cmd);
        executeCommand(cmd);
    }
}

void Screen::setRunning(bool running) {
    this->running = running;
}

void Screen::setProcess(std::shared_ptr<Process> process) {
    this->process = process;
}

void Screen::executeCommand(const std::string& cmd) {
    std::cout << std::endl;

    if (process == nullptr) {
        std::cout << "Process is still being generated." << std::endl;
    } else {
        if (cmd == "process-smi") {
            display();
            process->displayLog();
            std::cout << std::endl;

            if (!process->isFinished()) {
                std::cout << "Current instruction line: " << process->getLineNumber() << std::endl;
                std::cout << "Lines of code: " << process->getInstructionSize() << std::endl;
            } else {
                std::cout << "Finished!" << std::endl;
            }
        } else if (cmd == "exit") {
            ConsoleManager::getInstance()->switchConsole(MAIN_CONSOLE);
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }
}

void Screen::display() {
    std::cout << "Process name: " << process->getProcessName() << std::endl;
    std::cout << std::endl;
}

