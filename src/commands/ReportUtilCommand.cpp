
#include "../interfaces/Command.hpp"
#include "../services/ReportUtilService.cpp"
class ReportUtilCommand : public Command {
    public:
        ReportUtilCommand(ReportUtilService *service) : Command(*service) {}
        void print(std::string log) {
            std::cout << log << std::endl;
        }

        void execute(std::string input) {
            //do something
            std::string log = this->service.executeFlags(input);
            this->print(log);
        }
};

