
#include "../interfaces/Command.hpp"
#include "../services/SchedulerService.cpp"
class SchedulerCommand : public Command {
    public:
        SchedulerCommand(SchedulerService *service) : Command(*service) {}
        void print(std::string log) {
            std::cout << log << std::endl;
        }

        void execute(std::string input) {}

        void executeStart(std::string input) {
            //do something
            std::string log = this->service.executeFlags(input);
            this->print(log);
        }
        void executeStop(std::string input) {
            //do something
            std::string log = this->service.executeFlags(input);
            this->print(log);
        }
};

