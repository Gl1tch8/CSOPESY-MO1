#include "../../include/commands/SchedulerCommand.hpp" 

SchedulerCommand::SchedulerCommand(SchedulerService *service) : Command(*service) {}

void SchedulerCommand::print(std::string log) {
    std::cout << log << std::endl;
}

void SchedulerCommand::execute(std::string input) {}

void SchedulerCommand::executeStart(std::string input) {
    //do something
    std::string log = this->service.executeFlags(input);
    this->print(log);
}

void SchedulerCommand::executeStop(std::string input) {
    //do something
    std::string log = this->service.executeFlags(input);
    this->print(log);
}

