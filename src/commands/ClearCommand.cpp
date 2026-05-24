#include "../../include/commandshpp/ClearCommand.hpp" 

ClearCommand::ClearCommand(ClearService* service) : Command(*service) {}

void ClearCommand::print(std::string log) {
    std::cout << log << std::endl;
}

void ClearCommand::execute(std::string input) {
    std::string log = this->service.executeFlags(input);
    this->print(log);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    system("cls"); //clear the screen
}