#include "../../include/commands/ScreenCommand.hpp"
#include "tui/MainMenu.hpp"
#include <iostream>
#include <sstream>

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

    std::stringstream ss(input);
    std::string cmd, flag;
    ss >> cmd >> flag;

    std::string activeScreenName = screenService->getActiveScreen();

    MainMenuTUI::clear();


    if (flag == "-r") {
        for (const auto& line : screenService->getSessionLogs(activeScreenName)) {
            std::cout << line;
        }
    }

    // restore session history
    auto logAndPrint = [&](const std::string& text) {
        std::cout << text << std::endl;
        screenService->addSessionLog(activeScreenName, text + "\n");
    };

    while (screenService->hasActiveScreen()) {
        std::cout << "root:\\> ";
        std::getline(std::cin, input);
        if (!std::cin) {                 // EOF / closed stdin: leave the screen instead of spinning
            screenService->clearActiveScreen();
            break;
        }
        screenService->addSessionLog(activeScreenName, "root:\\> " + input + "\n");

        if (input == "process-smi") {
            logAndPrint(screenMuxService->processSMI(activeScreenName));
        } else if (input == "exit") {
            screenService->clearActiveScreen();
            std::cout << "Returning to main menu...\n";
        } else {
            logAndPrint("Unknown command. Use 'process-smi' or 'exit'.");
        }
    }
}