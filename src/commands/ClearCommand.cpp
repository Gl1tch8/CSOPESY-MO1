
#include "../interfaces/Command.hpp"
#include "../services/ClearService.cpp" //example: change this to /include/ClearService.hpp

// libraries for sleep
#include <chrono>
#include <thread>


class ClearCommand : public Command {
    public:
        ClearCommand(ClearService *service) : Command(*service) {}

        void print(std::string log) {
            std::cout << log << std::endl;
        }

        void execute(std::string input) {
            //do something
            std::string log = this->service.executeFlags(input);
            this->print(log);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            system("cls"); //clear the screen

            
        }
};

