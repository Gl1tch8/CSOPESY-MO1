#include "ClearService.cpp"
#include "Command.hpp"
class ClearCommand : public Command {
    public:
        void print(std::string log) {

        }
        void execute(std::string input) {
            //do something
            std::string log = this->service.executeFlags(input);
            this->print(log);
        }

};

