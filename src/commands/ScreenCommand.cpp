#include "../../include/commands/ScreenCommand.hpp" 
#include <iostream>

ScreenCommand::ScreenCommand(ScreenService *service) : Command(*service) {}

void ScreenCommand::print(std::string log) {
    std::cout << log << std::endl;
}

void ScreenCommand::execute(std::string input) {
    //do something
    std::string log = this->service.executeFlags(input);
    this->print(log);
}

