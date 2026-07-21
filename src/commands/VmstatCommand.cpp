#include "../../include/commands/VmstatCommand.hpp"

VmstatCommand::VmstatCommand(VmstatService* service) : Command(*service) {}

void VmstatCommand::print(std::string log) {
    std::cout << log << std::endl;
}

void VmstatCommand::execute(std::string input) {
    std::string log = this->service.executeFlags(input);
    this->print(log);
}
