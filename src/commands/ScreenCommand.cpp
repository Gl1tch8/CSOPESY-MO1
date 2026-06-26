#include "../../include/commands/ScreenCommand.hpp"
#include <iostream>

ScreenCommand::ScreenCommand(ScreenService* service) : Command(*service), screenService(service), screenMuxService(new ScreenMuxService()) {}

void ScreenCommand::print(std::string log) {
    std::cout << log << std::endl;
}

void ScreenCommand::execute(std::string input) {
    std::string log = this->service.executeFlags(input);
    if (!log.empty()) {
        this->print(log);
        return;
    }

    // -s was called, enter screen loop
    system("cls");
    this->print(screenMuxService->processSMI(screenService->getActiveScreen()));
    while (screenService->hasActiveScreen()) {
        std::cout << "root:\\> ";
        std::getline(std::cin, input);

        if (input == "process-smi") {
            this->print(screenMuxService->processSMI(screenService->getActiveScreen()));
        } else if (input == "exit") {
            screenService->clearActiveScreen();
            std::cout << "Returning to main menu...\n";
        } else {
            std::cout << "Unknown command. Use 'process-smi' or 'exit'.\n";
        }
    }
}