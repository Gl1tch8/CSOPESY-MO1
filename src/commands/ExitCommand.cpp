#include "../../include/commandshpp/ExitCommand.hpp" 

ExitCommand::ExitCommand(ExitService* service) : Command(*service) {}

void ExitCommand::print(std::string log) {
    std::cout << log << std::endl;
}

void ExitCommand::execute(std::string input) {
    std::string log = this->service.executeFlags(input);
    this->print(log);
}