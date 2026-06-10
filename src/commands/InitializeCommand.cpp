#include "../../include/commands/InitializeCommand.hpp" 

InitializeCommand::InitializeCommand(InitializeService* service) : Command(*service) {}

void InitializeCommand::print(std::string log) {
    std::cout << log << std::endl;
}

void InitializeCommand::execute(std::string input) {
    std::string log = this->service.executeFlags(input);
    this->print(log);
}
