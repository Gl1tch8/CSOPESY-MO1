#include "../../include/commands/ReportUtilCommand.hpp" 

ReportUtilCommand::ReportUtilCommand(ScreenService* service) : Command(*service) {}

void ReportUtilCommand::print(std::string log) {
    std::cout << log << std::endl;
        }

void ReportUtilCommand::execute(std::string input) {
    //do something
    std::string log = this->service.executeFlags(input);
    this->print(log);
}

